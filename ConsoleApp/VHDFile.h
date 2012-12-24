#pragma once
#include "W32Lib.h"

#pragma pack(push)
#pragma pack(1)
	
#define VHD_UNUSED_BAT_ENTRY						(DWORD)0xFFFFFFFF

typedef struct _VHD_FOOTER {
	char cookie[8];
	DWORD features;
	DWORD version;
	LONGLONG data_offset;
	DWORD time_stamp;
	DWORD creator_app;
	DWORD creator_ver;
	DWORD creator_os;
	LONGLONG original_size;
	LONGLONG current_size;
	DWORD disk_geometry;
	DWORD disk_type;
	DWORD checksum;
	BYTE id[16];
	BYTE saved_state;
	BYTE reserved[427];
} VHD_FOOTER;

enum DiskType {
	kFixedDisk = 2,
	kDynamicDisk,
	kDifferencingDisk
};

typedef struct _VHD_PARENT_LOCATOR {
	DWORD platform_code;
	DWORD platform_data_space;
	DWORD platform_data_length;
	DWORD reserved;
	LONGLONG platform_data_offset;
} VHD_PARENT_LOCATOR;

typedef struct _VHD_DYNAMIC_DISK_HEADER {
	char cookie[8];
	LONGLONG data_offset;
	LONGLONG table_offset;
	DWORD header_version;
	DWORD max_table_entries;
	DWORD block_size;
	DWORD checksum;
	BYTE parent_id[16];
	DWORD parent_time;
	DWORD reserved_1;
	BYTE parent_name[512];
	VHD_PARENT_LOCATOR entries[8];
	BYTE reserved_2[256];
} VHD_DYNAMIC_DISK_HEADER;

#pragma pack(pop)

//DWORD Be2Le(DWORD be_dw)
//{
//	DWORD le_dw;
//	BYTE *le = (BYTE *)&le_dw;
//	BYTE *be = (BYTE *)&be_dw;
//	le[0] = be[3];
//	le[1] = be[2];
//	le[2] = be[1];
//	le[3] = be[0];
//	return le_dw;
//}

class VHDFile
{
	FileEx *io;
public:
	VHDFile(const TCHAR *file_name)
	{
		io = new FileEx(file_name);
	}

	BOOL Open()
	{
		return io->Open();
	}
};


