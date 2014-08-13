#pragma once
#include "windows.h"
#include <QString>
#include <QFile>

class VDIFile
{
	enum ImageType {
		kDynamicDisk = 1,
		kDifferencingDisk = 2
	};

	typedef struct _HEADER {
		char creator_description[64];
		DWORD signature;
		WORD ver_min;
		WORD ver_maj;
		DWORD header_size;
		DWORD image_type;
		DWORD image_flags;
		char image_description[256];
		DWORD bat_offset;
		DWORD data_blocks_offset;
		DWORD cylinders;
		DWORD heads;
		DWORD sectors;
		DWORD sector_size;
		DWORD unused1;
		ULONGLONG disk_size;
		DWORD block_size;
		DWORD block_extra_data;
		DWORD max_blocks;
		DWORD allocated_blocks;
		UUID uuid;
		UUID last_snapshot_uuid;
		UUID link_uuid;
		UUID parent_uuid;
		char padding[56];
	} HEADER;

private:
	QFile *io;
	DWORD *bat;
	HEADER header;
	VDIFile *parent;
	bool InitializeBAT(void);
public:
	explicit VDIFile(QString vdi_file_name);
	~VDIFile(void);
	bool Open(void);
	void Close(void);
	void SetParent(VDIFile *parent_vdi);
	DWORD BlockSize(void);
	DWORD BlocksCount(void);
	bool ReadBlock(DWORD block_num, char *block_buff);
};

