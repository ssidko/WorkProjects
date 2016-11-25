#include "DhfsVolume.h"

DHFS::DhfsVolume::DhfsVolume(const std::string &volume_file) : io(volume_file, 256*512)
{
}

DHFS::DhfsVolume::~DhfsVolume()
{
}

bool DHFS::DhfsVolume::Open(void)
{
	if (io.Open()) {
		io.SetPointer(0x22 * 512LL);
		size_t rd = io.Read(&volume_header, sizeof(volume_header));
		io.SetPointer(0LL);
		return true;
	}
	return false;
}

bool DHFS::DhfsVolume::ReadFrame(Frame & frame)
{
	LONGLONG curr_offset = io.Pointer();

	FRAME_HEADER *header = nullptr;
	FRAME_FOOTER *footer = nullptr;
	frame.data.resize(sizeof(FRAME_HEADER));
	if (io.Read(&frame.data[0], sizeof(FRAME_HEADER)) == sizeof(FRAME_HEADER)) {
		header = (FRAME_HEADER *)frame.data.data();

		header->frame_size;
	
	}

	return false;
}
