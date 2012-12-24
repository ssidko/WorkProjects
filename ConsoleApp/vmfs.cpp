#include "vmfs.h"
#include <assert.h>

#define VMFS_SECTOR_SIZE			512


VMFS::VMFS(File *file, LONGLONG offset)
{
	assert(file);

	io = file;
	volume_offset = offset;

	memset(&volume_info, 0x00, sizeof(volume_info));
	memset(&lvm_info, 0x00, sizeof(lvm_info));

	if (io->Open()) {
		BYTE rw_buff[VMFS_SECTOR_SIZE] = {0};

		// Reading Volume Info
		io->SetPointer(volume_offset + VMFS_VOLUME_INFO_BASE);
		io->Read(&rw_buff, VMFS_SECTOR_SIZE);
		memcpy(&volume_info, rw_buff, sizeof(volume_info));

		// Reading LVM Info
		io->SetPointer(volume_offset + VMFS_VOLUME_INFO_BASE + VMFS_LVM_INFO_OFFSET);
		io->Read(&rw_buff, VMFS_SECTOR_SIZE);
		memcpy(&lvm_info, rw_buff, sizeof(lvm_info));

		// Reading FS Info
		io->SetPointer(volume_offset + VMFS_VOLUME_INFO_BASE + 32768LL*512 + VMFS_FS_INFO_BASE);
		io->Read(&rw_buff, VMFS_SECTOR_SIZE);
		memcpy(&fs_info, rw_buff, sizeof(fs_info));

		int x = 0;
	}	
}


VMFS::~VMFS(void)
{
}
