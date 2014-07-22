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
	UUID id;
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
	UUID parent_id;
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
private:
	QFile *io;
	VHD_FOOTER footer;
	VHD_DYNAMIC_DISK_HEADER dd_header;
	DWORD *bat;
	LONGLONG blocks_offset;
	DWORD sector_bitmap_size;
	DWORD sectors_per_block;
	DWORD max_block_number;
	bool opened;
	bool ReadFooter(LONGLONG offset);
	bool ReadDynamicDiskHeader(LONGLONG offset);
	bool InitializeBAT(void);
public:
	VHDFile(const QString &file_name);
	~VHDFile();
	bool Open();
	void Close();
	DWORD BlockSize(void);
	DWORD BlocksCount(void);
	DWORD SectorBitmapSize(void);
	DWORD SectorsPerBlock(void);
	bool ReadBlock(DWORD block_num, char *buff, DWORD size = 0);
};


