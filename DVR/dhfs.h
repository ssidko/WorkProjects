#ifndef _DHFS_H
#define _DHFS_H

#include "Timestamp.h"
#include "DhfsOnDisk.h"
#include "FileStorage.h"

namespace DHFS
{
	int StartRecovering(const std::string &dhfs_volume, const std::string &out_directory, const Timestamp &start_time, const Timestamp &end_time);
}

#endif // _DHFS_H