#include "ZfsObjectDataSaver.h"

#include <math.h>

ZfsObjectDataSaver::ZfsObjectDataSaver(W32Lib::FileEx &pool_io, dnode_phys_t &_dnode, const std::string & file_path) :
	io(pool_io),
	dnode(_dnode),
	out_file(file_path.c_str())
{
	if (!out_file.Create()) {
		throw std::exception("Error create out file.");
	}
	indblock_size = 1 << dnode.ind_blk_shift;
	pointers_per_indblock = indblock_size / sizeof(blkptr_t);
	data_block_size = dnode.data_blk_sz_sec * SPA_MINBLOCKSIZE;
}

ZfsObjectDataSaver::~ZfsObjectDataSaver()
{
}

int ZfsObjectDataSaver::Run()
{
	size_t level = dnode.nlevels - 1;

	if (!dnode.nblk_ptr) {
		return 0;
	}

	assert(level == dnode.blk_ptr[0].props.level);

	if (level == 0) {
		std::vector<char> buff;
		for (int i = 0; i < dnode.nblk_ptr; i++) {
			buff.clear();
			size_t expected_data_size = dnode.blk_ptr[i].props.embedded ? dnode.blk_ptr[i].props.logical_size + 1 : data_block_size;
			if (!ReadBlock(io, dnode.blk_ptr[i], buff)) {
				buff.resize(expected_data_size);
			}
			out_file.Write(buff.data(), buff.size());
		}
	} else {	
		size_t pointers_per_level = dnode.nblk_ptr;
		uint64_t blocks_count = dnode.max_blk_id + 1;
		uint64_t blocks_per_pointer = std::pow((uint64_t)pointers_per_indblock, (uint64_t)level);

		uint64_t full = blocks_count / blocks_per_pointer;
		assert(full <= pointers_per_indblock);
		for (int i = 0; i < full; i++) {
			SaveBlocks(dnode.blk_ptr[i], blocks_per_pointer, level);
		}

		uint64_t remainder = blocks_count % blocks_per_pointer;
		if (remainder) {
			SaveBlocks(dnode.blk_ptr[full], remainder, level);
		}	
	}

	return 0;
}

bool ZfsObjectDataSaver::SaveBlocks(blkptr_t & blkptr, uint64_t blocks_count, size_t level)
{
	assert(level && (blkptr.props.level == level));

	blkptr_t *bp = nullptr;
	uint64_t blocks_per_pointer = std::pow((uint64_t)pointers_per_indblock, (uint64_t)level - 1);

	assert(blocks_count <= (blocks_per_pointer * pointers_per_indblock));

	std::vector<char> pointers_buff;
	if (!ReadBlock(io, blkptr, pointers_buff)) {
		return false;
	}

	assert(pointers_buff.size() == indblock_size);

	if (level == 1) {

		std::vector<char> data_buff;
		data_buff.reserve(data_block_size);
		for(int i = 0; i < blocks_count; i++) {
			data_buff.clear();
			bp = &((blkptr_t *)pointers_buff.data())[i];
			if (!ReadBlock(io, *bp, data_buff)) {
				return false;
			}
			if (data_buff.size() != data_block_size);
			{
				data_buff.resize(data_block_size);
			}
			out_file.Write(data_buff.data(), data_block_size);		
		}
		return true;	

	} else {	
		uint64_t full = blocks_count / blocks_per_pointer;
		assert(full <= pointers_per_indblock);
		for (int i = 0; i < full; i++) {
			bp = &((blkptr_t *)pointers_buff.data())[i];
			SaveBlocks(*bp, blocks_per_pointer, level - 1);
		}
		uint64_t remainder = blocks_count % blocks_per_pointer;
		if (remainder) {
			bp = &((blkptr_t *)pointers_buff.data())[full];
			SaveBlocks(*bp, remainder, level - 1);
		}
		return true;
	}
}
