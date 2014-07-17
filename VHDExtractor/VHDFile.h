#pragma once
#include "windows.h"
#include <QFile>
#include <QString>

#pragma pack(push)
#pragma pack(1)
	
#define VHD_UNUSED_BAT_ENTRY						(DWORD)0xFFFFFFFF

typedef struct _VHD_DISK_GEOMETRY {
	WORD cylinder;
	BYTE heads;
	BYTE sectors_per_track;
} VHD_DISK_GEOMETRY;

typedef struct _VHD_FOOTER {
	char cookie[8];
	DWORD features;
	DWORD version;
	LONGLONG data_offset;
	DWORD time_stamp;
	BYTE creator_app[4];
	DWORD creator_ver;
	BYTE creator_os[4];
	LONGLONG original_size;
	LONGLONG current_size;
	VHD_DISK_GEOMETRY geometry;
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

#define VHD_SECTOR_SIZE				512

class VHDFile
{
	QFile *io;
	VHD_FOOTER footer;
	VHD_DYNAMIC_DISK_HEADER dd_header;
public:
	VHDFile(QString file_name);
	~VHDFile();

	bool Open();
	bool ReadFooter(void);
	bool ReadDynamicDiskHeader(void);
};


