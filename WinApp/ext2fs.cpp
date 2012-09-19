#include "ext2fs.h"

Ext2Volume::Ext2Volume(TCHAR *file_name, LONGLONG offset) : 
	file(NULL),
	volume_offset(offset)
{
	file = new File(file_name);
}

Ext2Volume::~Ext2Volume()
{
	delete file;
}

BOOL Ext2Volume::Open()
{
	ext2_super_block sb;
	if (file->Open())
	{
		file->SetPointer(volume_offset + EXT2_SUPERBLOCK_OFFSET);
		file->Read(&sb, sizeof(sb));

		int x = 0;
	}


	return TRUE;
}
