#ifndef _DHFS_H
#define _DHFS_H

#include "Timestamp.h"
#include "DhfsOnDisk.h"
#include "FileStorage.h"

namespace DHFS
{
	int StartRecovering(void);
}

#endif // _DHFS_H