#include "VHDFile.h"
#include <assert.h>
#include "BitMap.h"

WORD Be2Le(WORD be_w)
{
	WORD le_w;
	BYTE *le = (BYTE *)&le_w;
	BYTE *be = (BYTE *)&be_w;
	le[0] = be[1];
	le[1] = be[0];
	return le_w;
}

DWORD Be2Le(DWORD be_dw)
{
	DWORD le_dw;
	BYTE *le = (BYTE *)&le_dw;
	BYTE *be = (BYTE *)&be_dw;
	le[0] = be[3];
	le[1] = be[2];
	le[2] = be[1];
	le[3] = be[0];
	return le_dw;
}

LONGLONG Be2Le(LONGLONG be_ll)
{
	LONGLONG le_ll;
	BYTE *le = (BYTE *)&le_ll;
	BYTE *be = (BYTE *)&be_ll;
	le[0] = be[7];
	le[1] = be[6];
	le[2] = be[5];
	le[3] = be[4];
	le[4] = be[3];
	le[5] = be[2];
	le[6] = be[1];
	le[7] = be[0];
	return le_ll;
}

VHDFile::VHDFile(const QString &file_name) : opened(false), bat(NULL), blocks_offset(0),
		sector_bitmap_size(0), sectors_per_block(0), max_block_number(0), parent(NULL)
{
	io = new QFile(file_name);
	memset(&footer, 0x00, sizeof(VHD_FOOTER));
	memset(&dd_header, 0x00, sizeof(VHD_DYNAMIC_DISK_HEADER));
}

VHDFile::~VHDFile()
{
	Close();
}

bool VHDFile::ReadFooter(LONGLONG offset)
{
	assert(io);

	memset(&footer, 0x00, sizeof(VHD_FOOTER));

	if (!io->seek(offset)) return false;
	if (io->read((char *)&footer, sizeof(VHD_FOOTER)) == -1) return false;

	if (QByteArray(footer.cookie, 8) == QByteArray("conectix", 8)) {
		footer.features = Be2Le(footer.features);
		footer.version = Be2Le(footer.version);
		footer.data_offset = Be2Le(footer.data_offset);
		footer.time_stamp = Be2Le(footer.time_stamp);
		footer.creator_ver = Be2Le(footer.creator_ver);
		footer.original_size = Be2Le(footer.original_size);
		footer.current_size = Be2Le(footer.current_size);
		footer.geometry.cylinder = Be2Le(footer.geometry.cylinder);
		footer.disk_type = Be2Le(footer.disk_type);
		footer.checksum = Be2Le(footer.checksum);
		return true;
	} else {
		return false;
	}
}

bool VHDFile::ReadDynamicDiskHeader(LONGLONG offset)
{
	assert(io);

	memset(&dd_header, 0x00, sizeof(VHD_DYNAMIC_DISK_HEADER));

	if (!io->seek(offset)) return false;
	if (io->read((char *)&dd_header, sizeof(VHD_DYNAMIC_DISK_HEADER)) == -1) return false;

	if (QByteArray(dd_header.cookie, 8) == QByteArray("cxsparse", 8)) {
		dd_header.data_offset = Be2Le(dd_header.data_offset);
		dd_header.table_offset = Be2Le(dd_header.table_offset);
		dd_header.header_version = Be2Le(dd_header.header_version);
		dd_header.max_table_entries = Be2Le(dd_header.max_table_entries);
		dd_header.block_size = Be2Le(dd_header.block_size);
		dd_header.checksum = Be2Le(dd_header.checksum);
		dd_header.parent_time = Be2Le(dd_header.parent_time);
		for (int i = 0; i < 8; i++) {
			dd_header.entries[i].platform_code = Be2Le(dd_header.entries[i].platform_code);
			dd_header.entries[i].platform_data_space = Be2Le(dd_header.entries[i].platform_data_space);
			dd_header.entries[i].platform_data_length = Be2Le(dd_header.entries[i].platform_data_length);
			dd_header.entries[i].platform_data_offset = Be2Le(dd_header.entries[i].platform_data_offset);
		}
		return true;
	} else {
		return false;
	}
}

bool VHDFile::InitializeBAT(void)
{
	assert(!bat);

	if (!dd_header.max_table_entries) {
		return false;
	}
	bat = new DWORD[dd_header.max_table_entries];
	memset(bat, 0xFF, dd_header.max_table_entries);
	if (!io->seek(dd_header.table_offset)) {
		return false;
	}
	if (io->read((char *)bat, dd_header.max_table_entries*sizeof(DWORD)) == -1) {
		delete[] bat;
		bat = NULL;
		return false;
	}
	for (DWORD i = 0; i < dd_header.max_table_entries; i++) {
		bat[i] = Be2Le(bat[i]);
	}
	return true;
}

bool VHDFile::Open()
{
	Close();
	if (!io->open(QIODevice::ReadOnly)) {
		return false;
	}
	if (!ReadFooter(0)) {
		if (!ReadFooter(io->size() - 512)) {
			if (!ReadFooter(io->size() - 511)) {
				return false;
			}
		}
	}
	if ((footer.disk_type == kDynamicDisk) || (footer.disk_type == kDifferencingDisk)) {
		if (!ReadDynamicDiskHeader(footer.data_offset)) {
			if (!ReadDynamicDiskHeader(512)) {
				return false;
			}
		}
		if (!InitializeBAT()) {
			return false;
		}
		blocks_offset = footer.data_offset + sizeof(VHD_DYNAMIC_DISK_HEADER) + ((dd_header.max_table_entries*4)/512)*512;
		if ((dd_header.max_table_entries*4) % VHD_SECTOR_SIZE) {
			blocks_offset += VHD_SECTOR_SIZE;
		}
		sectors_per_block = dd_header.block_size/VHD_SECTOR_SIZE;
		sector_bitmap_size = sectors_per_block/8;
		if (sectors_per_block % 8) {
			sector_bitmap_size++;
		}
		if (sector_bitmap_size % VHD_SECTOR_SIZE) {
			sector_bitmap_size = (sector_bitmap_size/VHD_SECTOR_SIZE)*VHD_SECTOR_SIZE + VHD_SECTOR_SIZE;
		}
		max_block_number = dd_header.max_table_entries;
	} else if (footer.disk_type == kFixedDisk) {
		blocks_offset = 0;
		max_block_number = io->size()/VHD_SECTOR_SIZE - 1;
	}
	return opened = true;
}

void VHDFile::Close(void)
{
	if (io) {
		io->close();
	}
	if (bat) {
		delete[] bat;
		bat = NULL;
	}
	memset(&footer, 0x00, sizeof(VHD_FOOTER));
	memset(&dd_header, 0x00, sizeof(VHD_DYNAMIC_DISK_HEADER));
	opened = false;
	blocks_offset = 0;
	sector_bitmap_size = 0;
	sectors_per_block = 0;
}

void VHDFile::SetParent(VHDFile *parent_vhd)
{
	parent = parent_vhd;
}

DWORD VHDFile::BlockSize(void)
{
	if ((footer.disk_type == kDynamicDisk) || (footer.disk_type == kDifferencingDisk)) {
		return dd_header.block_size;
	} else {
		return VHD_SECTOR_SIZE;
	}
}

DWORD VHDFile::BlocksCount( void )
{
	if ((footer.disk_type == kDynamicDisk) || (footer.disk_type == kDifferencingDisk)) {
		return dd_header.max_table_entries;
	} else {
		return (DWORD)(io->size()/VHD_SECTOR_SIZE);
	}
}

DWORD VHDFile::SectorBitmapSize(void)
{
	return sector_bitmap_size;
}

DWORD VHDFile::SectorsPerBlock(void)
{
	return sectors_per_block; 
}

bool VHDFile::ReadBlock(DWORD block_num, char *block_buffer)
{
	assert(block_buffer);

	DWORD block_size = BlockSize();
	if (block_num < max_block_number) {
		if (footer.disk_type == kDynamicDisk) {
			if (bat[block_num] == VHD_UNUSED_BAT_ENTRY) {
				memset(block_buffer, 0x00, block_size);
				return true;		
			} else {
				if (io->seek((LONGLONG)((LONGLONG)bat[block_num]*VHD_SECTOR_SIZE + sector_bitmap_size))) {
					if (io->read(block_buffer, block_size) != -1) {
						return true;
					}
				}
			}
		} else if (footer.disk_type == kDifferencingDisk) {
			if (bat[block_num] == VHD_UNUSED_BAT_ENTRY) {
				if (parent) {
					return parent->ReadBlock(block_num, block_buffer);
				} else {
					memset(block_buffer, 0x00, block_size);
					return true;
				}
			} else {
				if (io->seek((LONGLONG)((LONGLONG)bat[block_num]*VHD_SECTOR_SIZE))) {
					bool result = false;
					char *tmp_buffer = NULL;
					char *parrent_block = NULL;
					char *this_block = NULL;
					char *bitmap_buffer = new char[sector_bitmap_size];
					memset(bitmap_buffer, 0x00, sector_bitmap_size);
					if (io->read(bitmap_buffer, sector_bitmap_size) != -1) {
						BitMap sectors_map(bitmap_buffer, sectors_per_block);
						DWORD count_1 = sectors_map.OneCount();
						if ((count_1 == sectors_per_block) || !parent) {
							if (io->read(block_buffer, block_size) != -1) {
								result = true;
							}
						} else {
							tmp_buffer = new char[block_size];
							memset(tmp_buffer, 0x00, block_size);
							this_block = block_buffer;
							parrent_block = tmp_buffer;
							if (count_1 < sectors_per_block/2) {
								this_block = tmp_buffer;
								parrent_block = block_buffer;
							}
							if (io->read(this_block, block_size) != -1) {
								if (parent->ReadBlock(block_num, parrent_block)) {
									for (DWORD i = 0; i < sectors_per_block; i++) {
										if (!sectors_map[i]) {
											memcpy((void *)block_buffer[i*VHD_SECTOR_SIZE], (void *)tmp_buffer[i*VHD_SECTOR_SIZE], VHD_SECTOR_SIZE);
										}
									}
									result = true;
								}
							}
						}
					}
					delete[] bitmap_buffer;
					if (tmp_buffer) {
						delete[] tmp_buffer;
					}
					return result;
				}
			}
		} else {
			if (io->seek((LONGLONG)block_num*VHD_SECTOR_SIZE)) {
				if (io->read(block_buffer, BlockSize()) != -1) {
					return true;
				}
			}
		}
	}
	return false;
}

DWORD VHDFile::UsedEntriesInBAT( void )
{
	DWORD counter = 0;
	for (int i = 0; i < dd_header.max_table_entries; i++) {
		if (bat[i] != VHD_UNUSED_BAT_ENTRY) {
			counter++;
		}
	}
	return counter;
}
