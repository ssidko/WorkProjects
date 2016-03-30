#include "FileRecordRecovery.h"

#define NTFS_MFT_FILE_RECORD_SIZE		1024

using namespace W32Lib;

int FileRecordRecoveryStart(void)
{
	FileEx file("D:\\Work\\My\\MFT");
	if (!file.Open()) {
		return -1;
	}

	BYTE buff[NTFS_MFT_FILE_RECORD_SIZE] = {0};
	DWORD next_attr_offset = 0;
	MFT_RECORD *record = nullptr;
	ATTR_RECORD *attribute = nullptr;

	while (file.Read(buff, NTFS_MFT_FILE_RECORD_SIZE) == NTFS_MFT_FILE_RECORD_SIZE) {
		record = (MFT_RECORD *)buff;

		if (record->magic == NTFS_RECORD_TYPES::magic_FILE) {
			next_attr_offset = record->attrs_offset;
			attribute = (ATTR_RECORD *)&buff[next_attr_offset];
			while ((attribute->attr_type != ATTR_TYPES::ATTR_END) || (next_attr_offset < NTFS_MFT_FILE_RECORD_SIZE)) {
			
				next_attr_offset += attribute->length;
				attribute = (ATTR_RECORD *)&buff[next_attr_offset];

				int x = 0;
			}


			int x = 0;		
		}
	
		record = (MFT_RECORD *)buff;
	}

	return 0;
}