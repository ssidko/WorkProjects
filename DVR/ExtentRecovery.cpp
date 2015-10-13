#include "ExtentRecovery.h"
#include <sstream>

ext4::ExtentSaver::ExtentSaver(const std::string &result_out_dir, const std::string &ext_volume_file, const LONGLONG &ext_volume_offset, DWORD ext_block_size) :
	out_dir(result_out_dir),
	volume_name(ext_volume_file),
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
			current_block++;
			break;
		}	
		current_block++;
	}

	delete[] buff;
 	return result;
}

bool ext4::ExtentSaver::SaveToFile(const LONGLONG &block_num, W32Lib::FileEx *out_file)
{
	W32Lib::FileEx io(volume_name.c_str());
	if (!io.Open()) {
		return false;
	}
	if (!io.SetPointer(volume_offset + (block_num * block_size))) {
		return false;
	}
	
	BYTE *block_buff = new BYTE[block_size];
	EXTENT_BLOCK *extent_block = (EXTENT_BLOCK *)block_buff;

	if (io.Read(block_buff, block_size) != block_size) {
		return false;
	}

	if ((extent_block->header.magic != EXTENT_HEADER_MAGIC) || (extent_block->header.max != max_extents_in_block) || (extent_block->header.entries > max_extents_in_block)) {
		return false;
	}

	std::vector<BYTE> data_buff;
	LONGLONG offset = 0;
	DWORD size = 0;
	if (extent_block->header.depth == 0) {
		for (int i = 0; i < extent_block->header.entries; i++) {
			offset = volume_offset + extent_block->extent[i].PysicalBlock() * block_size;

			size = (extent_block->extent[i].length <= 0x8000) ? (extent_block->extent[i].length * block_size) : ((extent_block->extent[i].length - 0x8000) * block_size);

			if (data_buff.size() < size) {
				data_buff.resize(size);
			}

			if (extent_block->extent[i].length <= 0x8000) {
				if (!io.SetPointer(offset)) {
					break;
				}
				if (!io.Read(data_buff.data(), size)) {
					break;
				}
			} else {
				memset(data_buff.data(), 0x00, size);			
			}

			if (!out_file->SetPointer((LONGLONG)extent_block->extent[i].block * block_size)) {
				break;
			}
			out_file->Write(data_buff.data(), size);
		}
	} else {
		for (int i = 0; i < extent_block->header.entries; i++) {
			SaveToFile(extent_block->extent_index[i].PysicalBlock(), out_file);
			int x = 0;
		}	
	}
	return true;
}

int ext4::ExtentSaver::Run()
{
	int file_counter = 0;
	LONGLONG block_num = 0;
	SetPointer(/*6143202ll*/75530359ll);
	while (NextExtentBlock(block_num)) {
		std::stringstream sstr;
		sstr << out_dir << "\\" << block_num;
		W32Lib::FileEx *out_file = new W32Lib::FileEx(sstr.str().c_str());
		if (out_file->Create()) {
			SaveToFile(block_num, out_file);
			file_counter++;
		}
		delete out_file;
	}
	return 0;
}
