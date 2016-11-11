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




	}

	return false;
}
