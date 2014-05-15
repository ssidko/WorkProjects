#pragma once

#include "W32Lib.h"
#include "windows.h"

#pragma pack(push)
#pragma pack(1)


#define VMFS_VOLUME_INFO_BASE				0x100000LL
#define VMFS_VOLUME_INFO_MAGIC				0xc001D00D

typedef struct _VMFS_VOLUME_INFO {
   DWORD magic;
   DWORD ver;
   BYTE unknown_0[6];
   BYTE lun;
   BYTE unknown_1[3];
   BYTE name[28];
   BYTE unknown_2[49];
   DWORD size;					// Size of the physical volume, divided by 256
   BYTE unknown_3[31];
   uuid_t uuid;
   ULONGLONG ctime;				// ctime? in usec
   ULONGLONG mtime;				// mtime? in usec
} VMFS_VOLUME_INFO;

#define VMFS_LVM_INFO_OFFSET					0x0200LL

typedef struct _VMFS_LVM_INFO {
   ULONGLONG size;
   ULONGLONG blocks;			// Seems to always be sum(num_segments for all extents) + num_extents
   DWORD unknown_0;
   BYTE uuid_str[35];
   BYTE unknown_1[29];
   uuid_t uuid;
   DWORD unknown_2;
   ULONGLONG ctime;				// ctime? in usec
   DWORD unknown_3;
   DWORD num_segments;
   DWORD first_segment;
   DWORD unknown_4;
   DWORD last_segment;
   DWORD unknown_5;
   ULONGLONG mtime;				// mtime? in usec
   DWORD num_extents;
} VMFS_LVM_INFO;

#define VMFS_FS_INFO_BASE						0x0200000LL
#define VMFS_FS_INFO_MAGIC						0x2fabf15e

typedef struct _VMFS_FS_INFO {
	DWORD magic;
	DWORD volver;
	BYTE ver;
	uuid_t uuid;
	DWORD mode;
	BYTE label[128];
	DWORD dev_blocksize;
	ULONGLONG blocksize;
	DWORD ctime; /* ctime? in seconds */
	DWORD unknown_3;
	uuid_t lvm_uuid;
	BYTE unknown_4[16];
	DWORD fdc_header_size;
	DWORD fdc_bitmap_count;
	DWORD subblock_size;
} VMFS_FS_INFO;

#pragma pack(pop)


class VMFSVolume : public IFile
{
private:
	File io;
	LONGLONG volume_offset;
	BOOL opened;
	VMFS_VOLUME_INFO volume_info;
	VMFS_LVM_INFO lvm_info;

	BOOL ReadVolumeInfo(VMFS_VOLUME_INFO *vi);
	BOOL ReadLVMInfo(VMFS_LVM_INFO *li);

public:
	VMFSVolume(const TCHAR *file_name, const LONGLONG *offset);
	~VMFSVolume();

	virtual BOOL Open(void);
	virtual BOOL Open(const TCHAR *file_name);
	virtual BOOL Create(void);
	virtual BOOL Create(const TCHAR *file_name);
	virtual BOOL IsOpen();
	virtual void Close(void);
	virtual DWORD Read(void *buffer, DWORD count);
	virtual DWORD Write(void *buffer, DWORD count);
	virtual BOOL SetPointer(LONGLONG pointer, DWORD dwMoveMethod = FILE_BEGIN);
	virtual BOOL GetPointer(LONGLONG *pointer);
};

class VMFS
{
private:
	VMFSVolume *volume;
	VMFS_FS_INFO fs_info;

	BOOL ReadFSInfo(void);
public:
	VMFS(VMFSVolume *vmfs_volume);
	~VMFS(void);

	BOOL Open(void);
	void Close(void) {}


};

