#include "G2fdbVolume.h"
#include <cassert>

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

	//
	// Необходима проверка на максимальный размер фрейма.
	//

	//#define MAX_FRAME_DATA_SIZE 4096*1024
	//if (header.data_size > MAX_FRAME_DATA_SIZE)) {
	//	return false;
	//}

	return true;
}

bool G2FDB::G2fdbVolume::ReadFrame(std::vector<BYTE>& buffer)
{
	size_t origin_size = buffer.size();
	FRAME_HEADER header = { 0 };

	if (FRAME_HEADER_SIZE == io.Read(&header, sizeof(FRAME_HEADER))) {
		if (IsValidFrameHeader(header)) {
			
			assert(header.data_size <= 64 * 1024);

			buffer.resize(buffer.size() + header.data_size);
			if (header.data_size == io.Read(&buffer[origin_size], header.data_size)) {
				
				return true;
			}
		}	
	}

	return false;
}
