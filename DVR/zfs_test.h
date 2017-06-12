#ifndef _ZFS_TEST_H
#define _ZFS_TEST_H

#include <memory>
#include <vector>
#include <map>

#include "zfs.h"
#include "VDev.h"
#include "File.h"

#include "dmu.h"
#include "dsl.h"
#include "zap.h"

#include "nvpair.h"
#include "Pool.h"

struct pool {

};

struct MetaObjecSet {
	objset_phys_t objset;
	void *object_dir;
};

struct Dataset {
	objset_phys_t objset;	
};


bool zfs_blkptr_verify(const blkptr_t &bp);

bool ReadBlock(W32Lib::FileEx &io, blkptr_t &blkptr, std::vector<char> &buffer);
bool ReadDataBlock(W32Lib::FileEx &io, dnode_phys_t &dnode, uint64_t block_num, std::vector<char> &buffer);

bool ObjectSet(W32Lib::FileEx &io, blkptr_t &blkptr);
bool ReadMOS(W32Lib::FileEx &io, blkptr_t &blkptr, MetaObjecSet &mos);

void zfs_test(void);

#endif // _ZFS_TEST_H
