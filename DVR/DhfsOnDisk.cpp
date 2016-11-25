#include <assert.h>
#include "DhfsOnDisk.h"
#include <sstream>
#include "File.h"

using namespace DHFS;

void DHFS::_FrameInfo::ToString( std::string &info_str )
{
	std::stringstream stream;
	stream << offset << " ";
	stream << timestamp.String() << " ";
	stream << "camera: " << camera << " ";
	stream << "flag: " << std::hex << "0x" <<  flag  << std::dec << " ";
	stream << "counter: " << counter << " ";
	stream << "\n";
	info_str = stream.str();
}

inline void DHFS::_FrameSequenceInfo::Clear(void)
{
	memset(this, 0x00, sizeof(_FrameSequenceInfo));
}

void DHFS::_FrameSequenceInfo::SetFirstFrame(FrameInfo & frame)
{
	frame_counter = 1;
	start_frame = frame;
	end_frame = frame;
	size = frame.frame_size;

	if ((frame.flag == 0xFC) || (frame.flag == 0xFD)) {
		start_sync_counter = frame.counter;
		end_sync_counter = frame.counter;
	}
}

//
// ѕопытка присоеденить следующий frame, дл€ этого 
//	- должна быть та же камера
//	- временна€ отметка фрейма должна быть равной последней временной отметке 
//	  последовательности или быть больше но не более чем на "1"
//	- sync_counter должен быть больше на "1"
//
bool DHFS::_FrameSequenceInfo::AppendFrame(FrameInfo & frame)
{
	if (start_frame.camera == frame.camera) {
		if (frame.timestamp.Seconds() >= end_frame.timestamp.Seconds()) {
			if ((frame.timestamp.Seconds() - end_frame.timestamp.Seconds()) <= 1) {

				if ((frame.flag == 0xFC) || (frame.flag == 0xFD)) {
					if (start_sync_counter == 0x00) {
						start_sync_counter = frame.counter;
						end_sync_counter = frame.counter;					
					} else {
						if ((frame.counter - end_sync_counter) == 1) {
							end_sync_counter = frame.counter;
						} else {
							return false;
						}
					}
				}

				end_frame = frame;
				frame_counter++;
				size += frame.frame_size;

				return true;
			}
		}
	}
	return false;
}

DHFS::Volume::Volume(const std::string &volume_file_name) : io(volume_file_name, 128*1024)
{
}

bool DHFS::Volume::Open()
{
	return io.Open();
}

void DHFS::Volume::SetPointer(const LONGLONG &pointer )
{
	io.SetPointer(pointer);
}

bool DHFS::Volume::IsValidHeader(FRAME_HEADER &header)
{
	if (header.magic == FRAME_HEADER_MAGIC) {
		if ((header.HeaderSize()) && (header.frame_size > (sizeof(FRAME_HEADER) + sizeof(FRAME_FOOTER))) && (header.frame_size <= FRAME_MAX_SIZE)) {
			return true;
		}
	}
	return false;
}

bool DHFS::Volume::FindAndReadNextFrame(std::vector<BYTE> &buffer, FrameInfo &info)
{
	LONGLONG offset = 0;
	DWORD header_magic = FRAME_HEADER_MAGIC;

	info.Clear();
	while ((offset = io.Find((BYTE *)&header_magic, sizeof(header_magic))) >= 0) {
		if (ReadFrame(buffer, info)) {
			return true;
		} else {
			io.SetPointer(++offset);
		}
	}
	return false;
}

bool DHFS::Volume::ReadFrame(std::vector<BYTE> &buffer, FrameInfo &info)
{
	FRAME_HEADER header = {0};
	FRAME_FOOTER footer = {0};
	size_t size = buffer.size();
	LONGLONG frame_offset = io.Pointer();

	info.Clear();
	if (io.Read(&header, sizeof(FRAME_HEADER)) == sizeof(FRAME_HEADER)) {
		if (IsValidHeader(header)) {
			
			info.camera = header.camera;
			info.flag = header.frame_type;
			info.offset = frame_offset;
			info.counter = header.sync_counter;
			info.frame_size = header.frame_size;
			header.time.Timestamp(info.timestamp);
			info.data_size = (header.frame_size - header.HeaderSize() - sizeof(FRAME_FOOTER));

			buffer.resize(buffer.size() + info.data_size);

			io.SetPointer(frame_offset + header.HeaderSize());

			if (io.Read(&buffer[size], info.data_size) == info.data_size) {
				if (io.Read(&footer, sizeof(FRAME_FOOTER)) == sizeof(FRAME_FOOTER)) {
					if ((footer.magic == FRAME_FOOTER_MAGIC) && (footer.frame_size == header.frame_size)) {
						return true;
					}
				}
			}

		} else {
			int y = 0;
		}
	}
	info.Clear();
	buffer.resize(size);
	io.SetPointer(frame_offset);

	return false;
}

bool DHFS::Volume::NextFrameSequence(std::vector<BYTE> &sequence_buffer, FrameSequenceInfo &sequence_info)
{
	bool result = false;
	FrameInfo frame_info;

	frame_info.Clear();
	sequence_info.Clear();
	sequence_buffer.clear();

	if (FindAndReadNextFrame(sequence_buffer, frame_info)) {

		sequence_info.SetFirstFrame(frame_info);

		while (result = ReadFrame(sequence_buffer, frame_info)) {

			if (sequence_info.AppendFrame(frame_info)) {
				continue;
			} else {
				sequence_buffer.resize(sequence_buffer.size() - frame_info.data_size);
				io.SetPointer(frame_info.offset);
				break;
			}		

		}

		if (!result) {
			io.SetPointer(sequence_info.end_frame.offset + 1);
		}
		return true;
	}
	return false;
}

void DHFS::Volume::Test(void)
{
	//FrameInfo info;
	FrameSequenceInfo info;
	std::vector<BYTE> buffer;
	buffer.reserve(4*1024*1024);

	while (NextFrameSequence(buffer, info)) {
		buffer.clear();
		int x = 0;
	}
}

void DHFS::Volume::SaveFrameInfo(const std::string &out_file)
{
	FrameInfo frame_info;
	std::string info_str;
	std::vector<BYTE> buffer;
	buffer.reserve(4*1024*1024);

	W32Lib::FileEx log(out_file.data());
	if (log.Create()) {
		while (FindAndReadNextFrame(buffer, frame_info)) {
			frame_info.ToString(info_str);
			log.Write((void *)info_str.c_str(), info_str.size());
			info_str.clear();
			buffer.clear();
		}
	}
}

void DHFS::Volume::SaveFrameSequenceInfo(const std::string &out_file)
{
	FrameInfo frame_info;
	std::string info;
	std::string out_str;
	std::vector<BYTE> buffer;
	buffer.reserve(4 * 1024 * 1024);

	W32Lib::FileEx log(out_file.data());
	if (log.Create()) {

		std::vector<BYTE> sequence;
		FrameSequenceInfo sequence_info;

		while (NextFrameSequence(sequence, sequence_info)) {

			out_str = "frames: ";
			out_str += std::to_string(sequence_info.frame_counter) + "; ";
			out_str += "size: ";
			out_str += std::to_string(sequence_info.size) + ";\n";

			log.Write((void *)out_str.c_str(), out_str.size());
			out_str.clear();
			
			out_str = "first: ";
			sequence_info.start_frame.ToString(info);
			out_str += info;

			log.Write((void *)out_str.c_str(), out_str.size());
			out_str.clear();

			out_str = "last:  ";
			sequence_info.end_frame.ToString(info);
			out_str += info;

			log.Write((void *)out_str.c_str(), out_str.size());
			out_str.clear();

			out_str = "===============================================================================\n";

			log.Write((void *)out_str.c_str(), out_str.size());
			out_str.clear();
	
		}
	}
}