#include "G2fdbVolume.h"

using namespace G2FDB;

G2fdbVolume::G2fdbVolume(const std::string &volume_file) : io(volume_file, 256*512)
{
}

G2fdbVolume::~G2fdbVolume()
{
}

bool G2fdbVolume::Open(void)
{
	return io.Open();
}

bool G2fdbVolume::SetPointer(LONGLONG &offset)
{
	return io.SetPointer(offset);
}

bool G2FDB::G2fdbVolume::IsValidFrameHeader(const FRAME_HEADER &header)
{
	if (header.signature != FRAME_HEADER_SIGNATURE_1) {
		return false;
	}

	if (header.signature_2 != FRAME_HEADER_SIGNATURE_2) {
		return false;
	}

	if (0x00 != std::memcmp(header.string, "CAM", 3)) {
		return false;
	}

	if (header.str_length > sizeof(FRAME_HEADER::string)) {
		return false;
	}

	return true;
}

bool G2FDB::G2fdbVolume::ReadFrame(std::vector<BYTE>& buffer)
{
	FRAME_HEADER header = { 0 };

	if (FRAME_HEADER_SIZE == io.Read(&header, sizeof(FRAME_HEADER))) {
		if (IsValidFrameHeader(header)) {
			
		}
	
	}

	return false;
}
