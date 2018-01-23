#pragma once
#include <exception>
#include <vector>
#include <string>
#include "zfs_test.h"

class ZfsObjectDataSaver
{
private:
	dnode_phys_t dnode;
	W32Lib::FileEx &io;
	W32Lib::FileEx out_file;

	uint64_t indblock_size;
	size_t pointers_per_indblock;
	size_t data_block_size;

public:
	ZfsObjectDataSaver(W32Lib::FileEx &pool_io, dnode_phys_t &_dnode, const std::string &file_path);
	~ZfsObjectDataSaver();
	int Run();
private:
	bool SaveBlocks(blkptr_t &blkptr, uint64_t blocks_count, size_t level);
};

