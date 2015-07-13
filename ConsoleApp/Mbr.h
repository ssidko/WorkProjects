#ifndef _MBR_H
#define _MBR_H

#include <windows.h>
#include "MbrPartitionTypes.h"

#define MBR_SIGNATURE				(WORD)0xAA55
#define MBR_MAX_PARTITIONS_COUNT	(WORD)4

#pragma pack(push)
#pragma pack(1) 

typedef struct _MBR_PARTITION {
	BYTE active_flag;
	BYTE start_head;
	BYTE start_sector;
	BYTE start_cylinder;
	BYTE type;
	BYTE end_head;
	BYTE end_sector;
	BYTE end_cylinder;
	DWORD offset;						// Partition offset in sectors.
	DWORD size;							// Partition size in sectors.
} MBR_PARTITION;

typedef struct _MBR {
	BYTE boot_code[446];
	MBR_PARTITION partition[MBR_MAX_PARTITIONS_COUNT];
	WORD signature;
} MBR;

#pragma pack(pop)


#endif // _MBR_H