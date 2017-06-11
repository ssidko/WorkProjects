#include "zfs_test.h"

#include <memory>
#include <map>
#include <system_error>
#include <string>
#include "sha256.h"
#include "lz4.h"

#define VDEV_OFFSET						2048*512
#define VDEV_LABEL_NVLIST_OFFSET		16*1024

bool ReadBlock(W32Lib::FileEx &io, blkptr_t &blkptr, std::vector<char> &buffer);
bool ReadDataBlock(W32Lib::FileEx &io, dnode_phys_t &dnode, uint64_t block_num, std::vector<char> &buffer);

void zfs_test(void)
{
	W32Lib::FileEx io;

	//if (!io.Open("D:\\zol-0.6.1\\vdev1")) {
	if (!io.Open("D:\\zfs\\zfs-flat.vmdk")) {
		return;
	}

	std::auto_ptr<uint8_t> vd_label_buff(new uint8_t[sizeof(vdev_label_t)]);
	vdev_label_t *label = (vdev_label_t *)vd_label_buff.get();

	NVList nvlist("vdev label");

	io.SetPointer(VDEV_OFFSET);
	io.Read(vd_label_buff.get(), sizeof(vdev_label_t));

	if (label->vl_vdev_phys.vp_zbt.zec_magic != ZEC_MAGIC) {
		// вай вай как не хорошо.
		int x = 0;
	}

	//zio_cksum_t old = label->vl_vdev_phys.vp_zbt.zec_cksum;
	//zio_cksum_t chksum = {0};

	//memset(&label->vl_vdev_phys.vp_nvlist, 0x00, sizeof(label->vl_vdev_phys.vp_zbt.zec_cksum));
	//zio_checksum_SHA256( &label->vl_vdev_phys.vp_nvlist, sizeof( label->vl_vdev_phys.vp_nvlist ), &chksum );


	DecodeXdrNVList(nvlist, (uint8_t *)label->vl_vdev_phys.vp_nvlist + 4, sizeof(label->vl_vdev_phys.vp_nvlist));

	uberblock_t *ub = nullptr;

	int max_tgx = 0;
	int ub_idx = 0;

	for (int i = 0; i < 128; ++i) {
		ub = (uberblock_t *)(label->vl_uberblock + i * 1024);
		if (ub->magic == UBERBLOCK_MAGIC) {
			if (ub->txg > max_tgx) {
				max_tgx = ub->txg;
				ub_idx = i;
			}			
		}	
	}

	ub = (uberblock_t *)(label->vl_uberblock + ub_idx * 1024);


	std::auto_ptr<objset_phys_t> objset(new objset_phys_t());

	io.SetPointer(VDEV_OFFSET + VDEV_DATA_OFFSET + ub->rootbp.dva[0].offset * 512);
	io.Read(objset.get(), sizeof(objset_phys_t));

	objset_phys_t *os = objset.get();


	//
	// Read object set, dnode array
	//
	uint64_t os_array_offset = os->os_meta_dnode.blk_ptr[0].dva[0].offset * 512;
	std::vector<char> os_array(os->os_meta_dnode.blk_ptr[0].dva[0].alloc_size * 512, 0);

	io.SetPointer(VDEV_OFFSET + VDEV_DATA_OFFSET + os_array_offset);
	io.Read(os_array.data(), os_array.size());

	size_t dn_count = os_array.size()/sizeof(dnode_phys_t);
	dnode_phys_t *dnode = (dnode_phys_t *)os_array.data();
	blkptr_t *bptr = (blkptr_t *)os_array.data();

	size_t asz = 0;

	bool result = false;
	std::vector<char> mos_buff;

	for (int i = 0; i <= os->os_meta_dnode.max_blk_id; ++i) {
		bptr = (blkptr_t *)os_array.data() + i;
		result = ReadBlock(io, *bptr, mos_buff);
		asz += bptr->dva[0].alloc_size;
	}

	dn_count = mos_buff.size()/sizeof(dnode_phys_t);


	//
	// Read MOS Object Directory
	//
	dnode = (dnode_phys_t *)mos_buff.data() + DMU_POOL_DIRECTORY_OBJECT;
	assert(dnode->type == DMU_OT_OBJECT_DIRECTORY);

	std::vector<char> zap_buff;
	bool res = ReadBlock(io, dnode->blk_ptr[0], zap_buff);

	std::map<std::string, uint64_t> mos_dir;
	TraversingMicroZapEntries(zap_buff, 
		[&mos_dir](const uint64_t &value, const char* name) {
		mos_dir.emplace(std::string(name), value);
	});

	//
	// Read root_dataset
	//
	uint64_t root_dataset_obj = mos_dir[DMU_POOL_ROOT_DATASET];
	assert(root_dataset_obj);

	dnode = (dnode_phys_t *)mos_buff.data() + root_dataset_obj;
	assert(dnode->type == DMU_OT_DSL_DIR);

	dsl_dir_phys_t *dir = (dsl_dir_phys_t *)dnode->bonus;

	dnode = (dnode_phys_t *)mos_buff.data() + dir->dd_child_dir_zapobj;

	zap_buff.clear();
	res = ReadBlock(io, dnode->blk_ptr[0], zap_buff);

	std::map<std::string, uint64_t> root_dir;
	TraversingMicroZapEntries(zap_buff, 
		[&root_dir](const uint64_t &value, const char* name) {
		root_dir.emplace(std::string(name), value);
	});


	dnode = (dnode_phys_t *)mos_buff.data() + dir->dd_head_dataset_obj;
	assert(dnode->type == DMU_OT_DSL_DATASET);

	dsl_dataset_phys_t *dataset = (dsl_dataset_phys_t *)dnode->bonus;


	std::vector<char> tmp;

	ReadBlock(io, dataset->ds_bp, tmp);

	objset_phys_t root_ds_objset = *(objset_phys_t *)tmp.data();

	tmp.clear();
	ReadDataBlock(io, root_ds_objset.os_meta_dnode, 0, tmp);

	size_t count = tmp.size() / sizeof(dnode_phys_t);
	for (int i = 0; i < count; i++) {
	
		dnode = (dnode_phys_t *)tmp.data() + i;
		dmu_object_type_t obj_type = (dmu_object_type_t)dnode->type;

		int x = 0;   	
	}


	int x = 0;
}

bool ReadBlock(W32Lib::FileEx &io, blkptr_t &blkptr, std::vector<char> &buffer)
{
	int result = 0;
	size_t decompressed_data_size = 0;
	size_t origin_size = buffer.size();
	std::vector<char> compressed_data;
	char *dst = nullptr;
	char *src = (char *)&blkptr;
	dnode_phys_t *dnode = nullptr;

	assert(blkptr.dva[0].gang_flag == 0);

	if (blkptr.props.embedded) {

		blk_props_emb_t *props = (blk_props_emb_t *)&blkptr.props;

		size_t logical_size = props->logical_size + 1;
		size_t physical_size = props->physical_size + 1;

		if (props->compression == ZIO_COMPRESS_OFF) {
			buffer.resize(origin_size + logical_size);
			dst = &buffer[origin_size];
		} else {
			compressed_data.resize(physical_size);
			dst = compressed_data.data();

			decompressed_data_size = logical_size;
			buffer.resize(origin_size + decompressed_data_size);
		}

		assert(props->physical_size <= 6*8 + 3*8 + 5*8);

		if (props->physical_size <= 6*8) {
			memcpy(dst, src, props->physical_size);
		} else {
			memcpy(dst, src, 6*8);
			if (props->physical_size <= 6*8 + 3*8) {
				memcpy(&dst[6*8], &src[6*8 + 8], props->physical_size - 6*8);			
			} else {
				memcpy(&dst[6*8], &src[6*8 + 8], 3*8);
				memcpy(&dst[6*8 + 3*8], &src[6*8 + 8 + 3*8 + 8], props->physical_size - 6*8 - 3*8);
			}		
		}

	} else {

		//
		// TODO: Validating dva, and if need read as HOLE.
		//

		size_t data_size = blkptr.dva[0].alloc_size * 512;

		if (blkptr.props.compression == ZIO_COMPRESS_OFF) {
			buffer.resize(origin_size + data_size);
			dst = &buffer[origin_size];
		} else {
			compressed_data.resize(data_size);
			dst = compressed_data.data();

			decompressed_data_size = (blkptr.props.logical_size + 1) * 512;
			buffer.resize(origin_size + decompressed_data_size);
		}

		if (!io.SetPointer(VDEV_OFFSET + VDEV_DATA_OFFSET + blkptr.dva[0].offset * 512)) {
			return false;
		}

		if (!io.Read(dst, data_size)) {
			return false;
		}
	}	

	switch (blkptr.props.compression) {
	
	case ZIO_COMPRESS_OFF:
		return true;

	case ZIO_COMPRESS_LZ4:
		result = lz4_decompress_zfs(compressed_data.data(), &buffer[origin_size], compressed_data.size(), decompressed_data_size, 0);
		assert(result == 0);
		return true;
	
	default :
		throw std::system_error(std::error_code(), "ReadBlock(): Unsupported compression.");
		return false;	
	}


	return false;
}

#include <cmath>

bool ReadDataBlock(W32Lib::FileEx &io, dnode_phys_t &dnode, uint64_t block_num, std::vector<char> &buffer)
{
	assert(dnode.nlevels);

	size_t ptr_per_block = (1 << dnode.ind_blk_shift) / sizeof(blkptr_t);
	size_t level = dnode.nlevels - 1;
	size_t blkptr_idx = 0;
	blkptr_t blkptr = {0};
	uint64_t bnum = block_num;

	std::vector<char> tmp;
	tmp.reserve(dnode.data_blk_sz_sec * SPA_MINBLOCKSIZE);

	uint64_t blocks_per_ptr = std::pow(ptr_per_block, level);
	uint64_t blocks_per_level = blocks_per_ptr * dnode.nblk_ptr;

	assert(block_num < blocks_per_level);

	blkptr = dnode.blk_ptr[block_num / blocks_per_ptr];
	do {
		assert(blkptr.props.level == level);

		tmp.clear();
		ReadBlock(io, blkptr, tmp);

		blocks_per_ptr = std::pow(ptr_per_block, level);

		assert(bnum < blocks_per_ptr * ptr_per_block);

		blkptr_idx = bnum / blocks_per_ptr;
		bnum -= bnum % blocks_per_ptr;
		blkptr = ((blkptr_t *)tmp.data())[blkptr_idx];

	} while (--level);

	assert(blkptr.props.level == 0);

	return ReadBlock(io, blkptr, buffer);
}


