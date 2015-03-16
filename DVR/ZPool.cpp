#include "ZPool.h"



ZPool::ZPool(const std::string &file_name, LONGLONG &volume_offset) :
	io(file_name),
	vol_offset(volume_offset)
{
}

ZPool::~ZPool(void)
{
}

bool ZPool::Open( void )
{
	return io.Open();
}

bool ZPool::ReadUberblock(uberblock_t &ublock, const LONGLONG &offset)
{
	io.SetPointer(vol_offset + offset);
	return (io.Read((void *)&ublock, sizeof(uberblock_t)) == sizeof(uberblock_t));
}

#include "compression.h"

void ZPool::Test( void )
{
	LONGLONG offset = 0;
	uberblock_t ublock;

	offset = 296*512;
	if (ReadUberblock(ublock, offset)) {
		int x = 0;
	}

	dnode_phys_t dnode;
	offset = (ublock.rootbp.dva[0].offset << 9) + 0x400000;
	if (ReadDNode(dnode, offset)) {
		int x = 0;
	}
	int x = 0;
}

bool ZPool::ReadDNode( dnode_phys_t &dnode, const LONGLONG &offset )
{
	io.SetPointer(vol_offset + offset);
	return (io.Read((void *)&dnode, sizeof(dnode_phys_t)) == sizeof(dnode_phys_t));
}
