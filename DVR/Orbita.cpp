#include "Orbita.h"
#include <assert.h>
#include <sstream>

using namespace Orbita;

int max_size = 0;

void ToMkv(std::string &raw_file_name, std::string &mkv_file_name)
{
	std::string t;
	//std::string convertor_app("C:\\Program Files\\MKVToolNix\\mkvmerge.exe ");
	std::string convertor_app("D:\\Soft\\#RecoverySoft#\\mkvtoolnix\\mkvmerge.exe ");

	std::stringstream cmd_line;
	cmd_line << convertor_app << "-o " << mkv_file_name << " " << raw_file_name;
	t = cmd_line.str();
	const char *zzz = t.data();
	system(cmd_line.str().data());
}

int Orbita::Main(const std::string &io_name, const std::string &out_dir)
{
	Orbita::Scanner scanner(io_name);
	FRAME_SEQUENCE sequence;
	if (scanner.Open()) {
		sequence.Clear();
		scanner.SetPointer(17898025*512LL);

		std::string out_file_name = "D:\\Work\\38702\\test\\sequence.h264";
		std::string mkv_file_name = out_file_name + ".mkv";
		W32Lib::FileEx out_file(out_file_name.c_str());

		if (out_file.Create()) {

			while (scanner.NextFrameSequence(sequence)) {
				

				if (sequence.first_frame.channel == 5) {
					if (out_file.GetSize() <= 500 * 1024*1024LL) {
						out_file.Write(sequence.buffer.data(), sequence.buffer.size());
					} else {
						out_file.Close();
						ToMkv(out_file_name, mkv_file_name);
						int x = 0;
					}

				}

				sequence.Clear();
			}
		}
	}

	return 0;
}

Orbita::Scanner::Scanner(const std::string &io_name) : io(io_name, 256*512)
{

}

Orbita::Scanner::~Scanner()
{
}

void Orbita::Scanner::AlignIoPointer(void)
{
	LONGLONG offset = io.Pointer();
	LONGLONG offset_mask = 7;
	offset = (offset & offset_mask) ? ((offset & ~offset_mask) + 8) : offset;
	io.SetPointer(offset);
}

bool Orbita::Scanner::IsValidFrameHeader(HEADER *header)
{
	switch (header->frame_type) {
	case 'cd':
		if (header->sub_type == 0x00) {
			return (((HEADER_0dc *)header)->IsValid());
		}
		else {
			return (((HEADER_dc *)header)->IsValid());
		}
	case 'bw':
		return (((HEADER_wb *)header)->IsValid());
	default:
		return false;
	}
}

DWORD Orbita::Scanner::HeaderExtraSize(HEADER *header)
{
	switch (header->frame_type) {
	case 'cd':
		if (header->sub_type == 0x00) {
			return (sizeof(HEADER_0dc) - sizeof(HEADER));
		}
		else {
			return (sizeof(HEADER_dc) - sizeof(HEADER));
		}
	case 'bw':
		return (sizeof(HEADER_wb) - sizeof(HEADER));
	default:
		return 0;
	}
}

DWORD Orbita::Scanner::FrameDataSize(HEADER *header)
{
	DWORD size = 0;
	switch (header->frame_type) {
	case 'cd':
		if (header->sub_type == 0x00) {
			size = (((HEADER_0dc *)header)->size);
		}
		else {
			size = (((HEADER_dc *)header)->size);
		}
		break;
	case 'bw':
		size = (((HEADER_wb *)header)->size_1);
		break;
	default:
		size = 0;
		break;
	}
	if (size > 0xF000) {
		size = 0;
	}
	return size;

}

Timestamp Orbita::Scanner::FrameTimestamp(HEADER *header)
{
	Timestamp t;
	if (header->frame_type == 'cd') {
		if (header->sub_type == 0x00) {
			t = ((HEADER_0dc *)header)->timestamp.Timestamp();
		}	
	}
	return t;
}

bool Orbita::Scanner::NextFrameHeader(LONGLONG &frame_offset)
{
	AlignIoPointer();

	DWORD size_buff = 64;
	LONGLONG *buff = new LONGLONG[size_buff];
	LONGLONG current_offset = io.Pointer();

	while (io.Read((void *)buff, 64 * sizeof(LONGLONG)) == (64 * sizeof(LONGLONG))) {
		for (DWORD i = 0; i < size_buff; i++) {
			HEADER *header = (HEADER *)&buff[i];
			if (header->IsValid()) {
				if (((HEADER_dc *)header)->IsValid() || ((HEADER_wb *)header)->IsValid()) {
					frame_offset = current_offset;
					return (io.SetPointer(frame_offset));
				}			
			}
			current_offset += 8;
		}	
	}
	return false;
}

bool Orbita::Scanner::NextFrameHeaderWithTimestamp(void)
{
	AlignIoPointer();

	DWORD size_buff = 64;
	LONGLONG *buff = new LONGLONG[size_buff];
	LONGLONG current_offset = io.Pointer();

	while (io.Read((void *)buff, 64 * sizeof(LONGLONG)) == (64 * sizeof(LONGLONG))) {
		for (DWORD i = 0; i < size_buff; i++) {
			HEADER *header = (HEADER *)&buff[i];
			if (((HEADER_dc *)header)->IsValid() && (header->sub_type == 0x00)) {
				delete[] buff;
				return (io.SetPointer(current_offset));
			}
			current_offset += 8;
		}
	}
	delete[] buff;
	return false;
}

bool Orbita::Scanner::ReadFrame(std::vector<BYTE> &buffer, FRAME_DESCRIPTOR &frame)
{
	HEADER *header;
	HEADER_dc *header_dc;
	HEADER_0dc *header_0dc;
	HEADER_wb *header_wb;

	DWORD data_size = 0;
	DWORD header_extra_size = 0;
	DWORD frame_start = buffer.size();

	frame.Clear();
	frame.offset = io.Pointer();

	buffer.resize(buffer.size() + sizeof(HEADER));
	if (io.Read(&buffer[frame_start], sizeof(HEADER)) == sizeof(HEADER)) {

		header = (HEADER *)&buffer[frame_start];
		if (header->IsValid()) {
			frame.channel = header->channel;
			frame.frame_type = header->frame_type;
			frame.sub_type = header->sub_type;

			header_extra_size = HeaderExtraSize(header);
			if (header_extra_size) {
			
				buffer.resize(buffer.size() + header_extra_size);
				if (io.Read(&buffer[frame_start + sizeof(HEADER)], header_extra_size) == header_extra_size) {
					
					header = (HEADER *)&buffer[frame_start];
					if (IsValidFrameHeader(header)) {

						data_size = FrameDataSize(header);
						if (data_size) {

							frame.size = sizeof(HEADER) + header_extra_size + data_size;
							frame.timestamp = FrameTimestamp(header);

							buffer.resize(buffer.size() + data_size);
							if (io.Read(&buffer[frame_start + sizeof(HEADER) + header_extra_size], data_size) == data_size) {
								AlignIoPointer();
								return true;
							}
						}
					}
				}			
			}
		}
	}

	buffer.resize(frame_start);
	io.SetPointer(frame.offset + 1);
	AlignIoPointer();

	return false;
}

bool Orbita::Scanner::NextFrameSequence(FRAME_SEQUENCE &sequence)
{
	FRAME_DESCRIPTOR current_frame;

	while (NextFrameHeaderWithTimestamp()) {
		
		sequence.Clear();
		while (ReadFrame(sequence.buffer, current_frame)) {

			if (sequence.frames_count == 0x00) {
				assert(current_frame.timestamp.Seconds());
				sequence.last_timestamp = current_frame.timestamp;
				sequence.first_frame = current_frame;
				sequence.last_frame = current_frame;
				sequence.frames_count++;
			} else {
				if (sequence.IsNextFrame(current_frame)) {

					if (current_frame.frame_type == 'bw') {
						sequence.buffer.resize(sequence.buffer.size() - current_frame.size);
						continue;
					}

					sequence.frames_count++;
					sequence.last_frame = current_frame;
					if (current_frame.timestamp.Seconds()) {
						sequence.last_timestamp = current_frame.timestamp;					
					}					
				} else {
					sequence.buffer.resize(sequence.buffer.size() - current_frame.size);
					io.SetPointer(current_frame.offset);
					break;
				}
			}
			if (sequence.frames_count == 42) {
				int x = 0;
			}
		}	
		if (sequence.frames_count)
			return true;
	}
	return false;
}
