#pragma once
#include <windows.h>

namespace fat {

#define LAST_LONG_ENTRY			(0x40)

	typedef enum {
		kReadOnly		= 1,
		kHidden			= 1 << 1,
		kSystem			= 1 << 2,
		kVolumeID		= 1 << 3,
		kDirectory		= 1 << 4,
		kArchive		= 1 << 5,
		kLongName		= kReadOnly|kHidden|kSystem|kVolumeID,
		kAttributesMask	= kLongName|kDirectory|kArchive,
	} Attributes;

#pragma pack(push)
#pragma pack(1)

	typedef struct _DATE {
		WORD day : 5;		// Day of month, valid range [1-31].
		WORD month : 4;	// Month of year, valid range [1-12].
		WORD year : 7;	// Count of years from 1980, valid range [0-127] (1980-2107).
	} DATE;

	typedef struct _TIME {
		WORD seconds : 5;	// valid range [0-29].
		WORD minutes : 6;	// valid range [0-59].
		WORD hours : 5;	// valid range [0-23].
	} TIME;

	typedef struct _DIR_ENTRY {
		BYTE name[11];
		BYTE attributes;
		BYTE nt_reserved;
		BYTE creation_time_tenth;
		TIME creation_time;
		DATE creation_date;
		WORD access_date;
		WORD first_cluster_hi;
		TIME write_time;
		DATE write_date;
		WORD first_cluster_lo;
		DWORD file_size;
	} DIR_ENTRY;

	typedef struct _DIR_ENTRY_LONG {
		BYTE order;
		BYTE name_1_5[10];
		BYTE attributes;
		BYTE type;
		BYTE checksum;
		BYTE name_6_11[12];
		WORD first_cluster_lo;		// must be 0x0000
		BYTE name_12_13[4];
	} DIR_ENTRY_LONG;

#pragma pack(pop)

	bool IsValidDirEntry(BYTE *buff);
	bool IsDotDirEntry(DIR_ENTRY *entry);
	bool IsDotDotDirEntry(DIR_ENTRY *entry);
	void EraseBadDirEntry(DIR_ENTRY *entry, DWORD count);
	int FindAndRepairDirectories(char *file_name, LONGLONG offset, DWORD cluster_size, DWORD max_cluster);

}
