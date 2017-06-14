#ifndef _ZFS_TEST_H
#define _ZFS_TEST_H

#include <memory>
#include <vector>
#include <string>
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
	objset_phys_t objset_phys;
	std::vector<dnode_phys_t> objset;
	std::map<std::string, uint64_t> object_dir;
	uint64_t root_dataset_obj;
};

struct Dataset {
	Dataset *parent;
	objset_phys_t objset_phys;
	std::vector<dnode_phys_t> objset;
	std::map<std::string, uint64_t> child_dir;
};


bool zfs_blkptr_verify(const blkptr_t &bp);

bool ReadBlock(W32Lib::FileEx &io, blkptr_t &blkptr, std::vector<char> &buffer);
bool ReadDataBlock(W32Lib::FileEx &io, dnode_phys_t &dnode, uint64_t block_num, std::vector<char> &buffer);

bool ReadDataset(W32Lib::FileEx &io, std::vector<dnode_phys_t> objset, uint64_t os_object, Dataset &dataset);
bool ReadMOS(W32Lib::FileEx &io, blkptr_t &blkptr, MetaObjecSet &mos);

void zfs_test(void);

#endif // _ZFS_TEST_H
