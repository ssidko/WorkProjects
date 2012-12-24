#include "vmfs.h"
#include <assert.h>

#define VMFS_SECTOR_SIZE			512

////////////////////////////////////////////////////////////////////////////////////
//									VMFSVolume
////////////////////////////////////////////////////////////////////////////////////
	
VMFSVolume::VMFSVolume(const TCHAR *file_name, const LONGLONG *offset) : io(file_name), opened(FALSE)
{
	volume_offset = *offset;
	memset(&volume_info, 0x00, sizeof(volume_info));
	memset(&lvm_info, 0x00, sizeof(lvm_info));
}

VMFSVolume::~VMFSVolume()
{
	Close();
}

BOOL VMFSVolume::ReadVolumeInfo(VMFS_VOLUME_INFO *vi)
{
	assert(vi);
	if (this->IsOpen()) {
		BYTE buff[VMFS_SECTOR_SIZE] = {0};
		io.SetPointer(volume_offset + VMFS_VOLUME_INFO_BASE);
		if (VMFS_SECTOR_SIZE == io.Read(buff, VMFS_SECTOR_SIZE)) {
			if (((VMFS_VOLUME_INFO *)buff)->magic == VMFS_VOLUME_INFO_MAGIC) {
				memcpy(vi, buff, sizeof(VMFS_VOLUME_INFO));
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL VMFSVolume::ReadLVMInfo(VMFS_LVM_INFO *li)
{
	assert(li);
	if (this->IsOpen()) {
		BYTE buff[VMFS_SECTOR_SIZE] = {0};
		io.SetPointer(volume_offset + VMFS_VOLUME_INFO_BASE + VMFS_LVM_INFO_OFFSET);
		if (VMFS_SECTOR_SIZE == io.Read(buff, VMFS_SECTOR_SIZE)) {
			memcpy(li, buff, sizeof(VMFS_LVM_INFO));
			return TRUE;
		}
	}
	return FALSE;
}

BOOL VMFSVolume::Open()
{
	if(io.Open()) {
		if (!ReadVolumeInfo(&volume_info))
			return FALSE;
		if (!ReadLVMInfo(&lvm_info))
			return false;
		return io.SetPointer(volume_offset);
	}
	return FALSE;
}

BOOL VMFSVolume::Open(const TCHAR *file_name)
{
	volume_offset = 0;
	if(io.Open(file_name)) {
		if (!ReadVolumeInfo(&volume_info))
			return FALSE;
		if (!ReadLVMInfo(&lvm_info))
			return false;
		return io.SetPointer(volume_offset);
	}
	return FALSE;
}

BOOL VMFSVolume::Create(void)
{
	return FALSE;
}

BOOL VMFSVolume::Create(const TCHAR *file_name)
{
	return FALSE;
}

BOOL VMFSVolume::IsOpen()
{
	return opened;
}

void VMFSVolume::Close()
{
	io.Close();
	opened = FALSE;
}

DWORD VMFSVolume::Read(void *buffer, DWORD count)
{
	return io.Read(buffer, count);
}

DWORD VMFSVolume::Write(void *buffer, DWORD count)
{
	return io.Write(buffer, count);
}

BOOL VMFSVolume::SetPointer(LONGLONG pointer, DWORD dwMoveMethod)
{
	LONGLONG offs = (LONGLONG)(volume_offset + VMFS_VOLUME_INFO_BASE + 0x1000000 + pointer);
	return io.SetPointer(offs);
}

BOOL VMFSVolume::GetPointer(LONGLONG *pointer)
{
	return io.GetPointer(pointer);
}



////////////////////////////////////////////////////////////////////////////////////
//										VMFS
////////////////////////////////////////////////////////////////////////////////////

VMFS::VMFS(VMFSVolume *vmfs_volume) : volume(vmfs_volume)
{
}

VMFS::~VMFS(void)
{
}

BOOL VMFS::ReadFSInfo(void)
{
	BYTE buff[VMFS_SECTOR_SIZE] = {0};
	if (volume->SetPointer(VMFS_FS_INFO_BASE)) {
		if (VMFS_SECTOR_SIZE == volume->Read(buff, VMFS_SECTOR_SIZE)) {
			if (((VMFS_FS_INFO *)buff)->magic == VMFS_FS_INFO_MAGIC) {
				memcpy(&fs_info, buff, sizeof(VMFS_FS_INFO));
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL VMFS::Open()
{
	if (!volume->IsOpen())
		if (!volume->Open())
			return FALSE;

	return ReadFSInfo();
}
