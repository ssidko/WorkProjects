#include "VDIFile.h"

#define VDI_HEADER_SIGNATURE					(DWORD)0xBEDA107F
#define VDI_UNUSED_BAT_ENTRY					(DWORD)0xFFFFFFFF

VDIFile::VDIFile(QString vdi_file_name) : io(NULL), bat(NULL), parent(NULL)
{
	io = new QFile(vdi_file_name);
	memset(&header, 0x00, sizeof(HEADER));
}

VDIFile::~VDIFile(void)
{
	Close();
}

bool VDIFile::Open()
{
	int x = sizeof(HEADER);
	if (io->open(QIODevice::ReadOnly)) {
		if ((qint64)sizeof(HEADER) == io->read((char *)&header, sizeof(HEADER))) {
			if (header.signature != VDI_HEADER_SIGNATURE) {
				return false;
			}
			if ((ULONGLONG)header.block_size*(ULONGLONG)header.max_blocks > header.disk_size) {
				return false;
			}
			return InitializeBAT();
		}
	}
	return false;
}

void VDIFile::Close()
{
	if (parent) {
		parent->Close();
		delete parent;
		parent = NULL;
	}
	if (bat) {
		delete[] bat;
		bat = NULL;
	}
	memset(&header, 0x00, sizeof(HEADER));
	io->close();
}

void VDIFile::SetParent(VDIFile *parent_vdi)
{
	if (parent) {
		parent->Close();
		delete parent;
		parent = NULL;
	}
	parent = parent_vdi;
}

bool VDIFile::InitializeBAT(void)
{
	bat = new DWORD[header.max_blocks];
	if (!bat) {
		return false;
	}
	memset((char *)bat, 0x00, header.max_blocks*sizeof(DWORD));
	if (io->seek((qint64)header.bat_offset)) {
		if ((qint64)header.max_blocks*sizeof(DWORD) == io->read((char *)bat, header.max_blocks*sizeof(DWORD))) {
			return true;
		}
	}
	return false;
}

DWORD VDIFile::BlockSize( void )
{
	return header.block_size;
}


DWORD VDIFile::BlocksCount( void )
{
	return header.max_blocks;
}

bool VDIFile::ReadBlock(DWORD block_num, char *block_buff)
{
	if (block_num >= header.max_blocks) {
		return false;
	}
	if (bat[block_num] >= header.max_blocks) {
		memset(block_buff, 0x00, header.block_size);
		return true;
	}
	if (header.image_type == kDynamicDisk) {
		if (bat[block_num] == VDI_UNUSED_BAT_ENTRY) {
			memset(block_buff, 0x00, header.block_size);
			return true;
		} else {
			if (io->seek((qint64)header.data_blocks_offset + (qint64)header.block_size*bat[block_num])) {
				if (io->read(block_buff, header.block_size) != -1) {
					return true;
				}
			}
		}
	} else if (header.image_type == kDifferencingDisk) {
		if (bat[block_num] == VDI_UNUSED_BAT_ENTRY) {
			if (parent) {
				return parent->ReadBlock(block_num, block_buff);
			} else {
				memset(block_buff, 0x00, header.block_size);
				return true;
			}
		} else {
			if (io->seek((qint64)header.data_blocks_offset + (qint64)header.block_size*block_num)) {
				if (io->read(block_buff, header.block_size) != -1) {
					return true;
				}
			}
		}
	}
	return false;
}
