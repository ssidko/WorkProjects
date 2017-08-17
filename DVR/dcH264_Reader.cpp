#include "dcH264_Reader.h"
#include <assert.h>

using namespace dcH264;

int dcH264::main(void)
{
	FRAME_HEADER *hdr = nullptr;
	LONGLONG disk_size = 1953525168ll * 512ll;

	Reader reader("\\\\.\\PhysicalDrive2", disk_size);
	if (reader.Open()) {

		std::vector<uint8_t> frame_buffer;
		bool result = reader.ReadFrame(0x6C71A83780, frame_buffer);
		int x = 0;
	}

	return 0;
}

inline LONGLONG Align(const LONGLONG &value, size_t align)
{
	return  ((value + (align - 1)) & ~(align - 1));
}

dcH264::Reader::Reader(const std::string &file_name, const LONGLONG &file_size) : io(file_name, 128 * 1024, file_size)
{
}

bool dcH264::Reader::Open()
{
	return io.Open();
}

bool dcH264::Reader::SetOffset(const LONGLONG &offset)
{
	return io.SetPointer(offset);
}

LONGLONG dcH264::Reader::Offset(void)
{
	return io.Pointer();
}

bool dcH264::Reader::ReadFrame(const LONGLONG &offset, std::vector<uint8_t> &buffer)
{
	FRAME_HEADER *hdr = nullptr;

	if (!io.SetPointer(offset)) {
		return false;
	}

	buffer.clear();

	//
	// Read FRAME_HEADER
	//

	buffer.resize(sizeof(FRAME_HEADER));	
	hdr = (FRAME_HEADER *)&buffer[0];

	if (buffer.size() != io.Read(&buffer[0], buffer.size())) {
		return false;
	}
	if (!hdr->IsValid()) {
		return false;
	}

	//
	// Read FRAME_PAYLOAD
	//
	
	size_t frame_size = hdr->FrameSize();

	buffer.resize(frame_size);
	hdr = (FRAME_HEADER *)&buffer[0];

	if (frame_size > sizeof(FRAME_HEADER)) {		
		size_t tail_size = frame_size - sizeof(FRAME_HEADER);
		if (tail_size != io.Read(&buffer[sizeof(FRAME_HEADER)], tail_size)) {
			return false;
		}		
	}

	//
	// Align file pointer.
	//

	if (!io.SetPointer(Align(offset + frame_size, 8))) {
		return false;
	}

	return true;
}