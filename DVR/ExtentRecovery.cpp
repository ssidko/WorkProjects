#include "ExtentRecovery.h"

int ext4::StartExtentRecovery(std::string ext_volume, const LONGLONG &start_offset, DWORD block_size, std::string out_dir)
{
	WORD max_extents_in_block = (block_size - sizeof(EXTENT_HEADER)) / sizeof(EXTENT);
	LONGLONG offset = start_offset;
	BufferedFile volume(ext_volume, 256*512);

	if (volume.Open()) {

		BYTE *block_buff = new BYTE[block_size];
		EXTENT_HEADER *extent_header = (EXTENT_HEADER *)block_buff;
		
		volume.SetPointer(offset);
		while (volume.Read(block_buff, block_size) == block_size) {
			if ((extent_header->magic == EXTENT_HEADER_MAGIC) && (extent_header->max == max_extents_in_block) && (extent_header->entries <= max_extents_in_block)) {

				int x = 0;

			}		
			offset += block_size;
			volume.SetPointer(offset);
		}

	}

	return 0;
}

ext4::ExtentSaver::ExtentSaver(const std::string &result_out_dir, const std::string &ext_volume_file, const LONGLONG &ext_volume_offset, DWORD ext_block_size) :
	out_dir(result_out_dir),
	io(ext_volume_file, 256*512),
	volume_offset(ext_volume_offset),
	block_size(ext_block_size),
	current_block(0)
{
	max_extents_in_block = (block_size - sizeof(EXTENT_HEADER)) / sizeof(EXTENT);
}

ext4::ExtentSaver::~ExtentSaver(void)
{
}

bool ext4::ExtentSaver::Open(void)
{
	current_block = 0;
	return (io.Open() && io.SetPointer(volume_offset));
}

bool ext4::ExtentSaver::SetPointer(const LONGLONG &block)
{
	current_block = block;
	return io.SetPointer( (volume_offset + (block*block_size)) );
}

bool ext4::ExtentSaver::NextExtentBlock(LONGLONG &block_num)
{
	bool result = false;
	BYTE *buff = new BYTE[block_size];
	EXTENT_BLOCK *extent_block = (EXTENT_BLOCK *)buff;

	SetPointer(current_block);
	while (io.Read(buff, block_size) == block_size) {
		if ((extent_block->header.magic == EXTENT_HEADER_MAGIC) && (extent_block->header.max == max_extents_in_block) && (extent_block->header.entries <= max_extents_in_block)) {
			result = true;
			block_num = current_block;
			break;
		}	
		current_block++;
	}

	delete[] buff;
 	return result;
}
