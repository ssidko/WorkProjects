#include "dcH264_Reader.h"
#include <assert.h>

using namespace dcH264;

int dcH264::main(void)
{
	FRAME_HEADER *hdr = nullptr;

	Reader reader("\\\\.\\PhysicalDrive2", 1953525168ll * 512ll);
	if (reader.Open()) {



	}

	return 0;
}

dcH264::Reader::Reader(const std::string &file_name, const LONGLONG &file_size) : io(file_name, 128 * 1024, file_size)
{
}

bool dcH264::Reader::Open()
{
	return io.Open();
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

	size_t frame_tail = 0;



	


	return true;
}

