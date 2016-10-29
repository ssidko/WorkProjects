#include "G2fdbVolume.h"
#include <cassert>

using namespace G2FDB;

void G2FDB::FrameInfo::Clear(void)
{
	offset = 0;
	camera = 0;
	time.Clear();
	data_size = 0;
}

DWORD G2FDB::FrameInfo::FullSize(void)
{
	return (data_size + FRAME_HEADER_SIZE);
}

G2FDB::G2fdbVolume::G2fdbVolume(const std::string &volume_file) : io(volume_file, 256*512)
{
}

G2FDB::G2fdbVolume::~G2fdbVolume()
{
}

bool G2FDB::G2fdbVolume::Open(void)
{
	return io.Open();
}

bool G2FDB::G2fdbVolume::SetPointer(LONGLONG &offset)
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

	assert(header.data_size <= 64 * 1024);

	return true;
}

bool G2FDB::G2fdbVolume::FindNextFrame(FrameInfo &frame_info)
{
	LONGLONG offset = 0;
	DWORD signature = FRAME_HEADER_SIGNATURE_2;
	BYTE *str = (BYTE *)&signature;

	if (io.Find(str, sizeof(signature))) {
	


	}

	return false;
}

bool G2FDB::G2fdbVolume::ReadFrame(FrameInfo &frame_info, vector<BYTE> &data)
{
	size_t origin_size = data.size();
	FRAME_HEADER header = { 0 };

	frame_info.offset = io.Pointer();
	if (FRAME_HEADER_SIZE == io.Read(&header, sizeof(FRAME_HEADER))) {
		if (IsValidFrameHeader(header)) {
			
			data.resize(data.size() + header.data_size);
			if (header.data_size == io.Read(&data[origin_size], header.data_size)) {
				
				frame_info.camera = header.camera;
				header.time.Timestamp(frame_info.time);
				frame_info.data_size = header.data_size;

				return true;
			}
		}	
	}

	io.SetPointer(frame_info.offset);
	data.resize(origin_size);
	frame_info.Clear();

	return false;
}

bool G2FDB::G2fdbVolume::ReadFrameSequence(FrameSequence &sequence)
{
	return false;
}
