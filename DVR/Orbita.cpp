#include "Orbita.h"

using namespace Orbita;

int Orbita::Main(const std::string &io_name, const std::string &out_dir)
{
	Orbita::Scanner scanner(io_name);
	if (scanner.Open()) {
		LONGLONG offset;
		FRAME_DESCRIPTOR frame;
		std::vector<BYTE> frame_sequence;
		while (scanner.NextFrame(offset)) {
			frame_sequence.clear();
			scanner.SetPointer(offset);
			while (scanner.ReadFrame(frame_sequence, frame)) {
				

				
				scanner.AlignIoPointer();
			}		
			scanner.SetPointer(++offset);
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

bool Orbita::Scanner::NextFrame(LONGLONG &frame_offset)
{
	LONGLONG o = io.Pointer();
	AlignIoPointer();
	LONGLONG current_offset = io.Pointer();

	DWORD size_buff = 64;
	LONGLONG *buff = new LONGLONG[size_buff];

	while (io.Read((void *)buff, 64 * sizeof(LONGLONG)) == (64 * sizeof(LONGLONG))) {
		for (DWORD i = 0; i < size_buff; i++) {
			HEADER *header = (HEADER *)&buff[i];
			if (header->IsValid()) {
				if (((HEADER_dc *)header)->IsValid() || ((HEADER_wb *)header)->IsValid()) {
					frame_offset = current_offset;
					return true;
				}			
			}
			current_offset += 8;
		}	
	}
	return false;
}

bool Orbita::Scanner::ReadFrame(std::vector<BYTE> &buffer, FRAME_DESCRIPTOR &frame)
{
	HEADER *header;
	HEADER_dc *header_dc;
	HEADER_0dc *header_0dc;
	HEADER_wb *header_wb;

	DWORD data_size = 0;
	DWORD header_tail_size = 0;
	DWORD frame_start = buffer.size();
	buffer.resize(buffer.size() + sizeof(HEADER));

	frame.Clean();

	frame.offset = io.Pointer();

	if (io.Read(&buffer[frame_start], sizeof(HEADER)) == sizeof(HEADER)) {

		header = (HEADER *)&buffer[frame_start];
		if (!header->IsValid()) {
			buffer.resize(frame_start);
			return false;
		}

		frame.channel = header->channel;
		frame.frame_type = header->frame_type;
		frame.sub_type = header->sub_type;
		
		switch (header->frame_type) {
			case 'cd':
				if (header->sub_type == 0x00) {
					header_tail_size = sizeof(HEADER_0dc) - sizeof(HEADER);
				} else {
					header_tail_size = sizeof(HEADER_dc) - sizeof(HEADER);
				}
				break;

			case 'bw':
				header_tail_size = sizeof(HEADER_wb) - sizeof(HEADER);
				break;

			default:
				buffer.resize(frame_start);
				return false;
				break;
		}

		buffer.resize(buffer.size() + header_tail_size);

		if (io.Read(&buffer[frame_start + sizeof(HEADER)], header_tail_size) != header_tail_size) {
			buffer.resize(frame_start);
			return false;
		}

		header = (HEADER *)&buffer[frame_start];

		switch (header->frame_type) {
			case 'cd':
				if (header->sub_type == 0x00) {
					data_size = ((HEADER_0dc *)header)->size;
					frame.timestamp = ((HEADER_0dc *)header)->timestamp;
				}
				else {
					data_size = ((HEADER_dc *)header)->size;
				}
				break;

			case 'bw':
				data_size = ((HEADER_wb *)header)->size_1;
				break;

			default:
				buffer.resize(frame_start);
				return false;
				break;
		}

		frame.size = sizeof(HEADER) + header_tail_size + data_size;

		buffer.resize(buffer.size() + data_size);

		if (io.Read(&buffer[frame_start + sizeof(HEADER) + header_tail_size], data_size) != data_size) {
			buffer.resize(frame_start);
			return false;
		}

		return true;
	
	}
	return false;
}
