#pragma once

#include "zfs.h"
#include "windows.h"
#include "BufferedFile.h"
#include <string>


class ZPool
{
private:
	BufferedFile io;
	LONGLONG vol_offset;
public:
	ZPool(const std::string &file_name, LONGLONG &volume_offset);
	~ZPool(void);
	bool Open(void);
	bool ReadUberblock(uberblock_t &ublock, const LONGLONG &offset);
	bool ReadDNode(dnode_phys_t &dnode, const LONGLONG &offset);

	void Test(void);
};

