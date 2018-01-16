#include "zfs_test.h"
#include <intrin.h>

#include <system_error>
#include <string>
#include <cmath>
#include "sha256.h"
#include "flatcher.h"
#include "lz4.h"
#include "lzjb.h"

//#define VDEV_OFFSET						323510272*512ULL
#define VDEV_OFFSET						323510272*512ULL
//#define VDEV_OFFSET						2048*512ULL
#define VDEV_LABEL_NVLIST_OFFSET		16*1024
#define SECTOR_SIZE_SHIFT				9
#define SECTOR_SIZE						(1 << SECTOR_SIZE_SHIFT)

size_t vdev_sector_size = SECTOR_SIZE;

inline bool	ChecksumEqual(zio_cksum &zc1, zio_cksum &zc2)
{
	return 	(0 == ( ((zc1).word[0] - (zc2).word[0]) | ((zc1).word[1] - (zc2).word[1]) |
		((zc1).word[2] - (zc2).word[2]) | ((zc1).word[3] - (zc2).word[3])) );
}


bool IsValidDnode(dnode_phys_t *dnode)
{
	if (dnode->type == DMU_OT_NONE) {
		return false;
	}

	if (!DMU_OT_IS_VALID(dnode->type)) {
		return false;
	}

	if (dnode->ind_blk_shift < DN_MIN_INDBLKSHIFT) return false;
	if (dnode->ind_blk_shift > 17) return false; // 128 Kb

	if (dnode->nlevels == 0) return false;
	if (dnode->nlevels > 6) return false;

	if (dnode->nblk_ptr > 3) return false;

	for (size_t i = 0; i < dnode->nblk_ptr; i++) {

		if (dnode->blk_ptr[i].props.embedded) {
			if (!zfs_blkptr_verify(dnode->blk_ptr[i])) return false;
		} else if (dnode->blk_ptr[i].dva[0].alloc_size) {
			if (!zfs_blkptr_verify(dnode->blk_ptr[i])) return false;
		}

	}

	if (dnode->checksum >= ZIO_CHECKSUM_FUNCTIONS) {
		return false;
	}

	if (dnode->compress >= ZIO_COMPRESS_FUNCTIONS) {
		return false;
	}

	//	Is dn_used in bytes?  if not, it's in multiples of SPA_MINBLOCKSIZE
	//#define	DNODE_FLAG_USED_BYTES			(1<<0)
	//#define	DNODE_FLAG_USERUSED_ACCOUNTED	(1<<1)
	//
	//	Does dnode have a SA spill blkptr in bonus?
	//#define	DNODE_FLAG_SPILL_BLKPTR			(1<<2)

	if (dnode->flags > 7) {
		return false;
	}

	if (dnode->nblk_ptr == 1) {
		if (dnode->bonus_type == DMU_OT_NONE) {
			return false;
		}

		if (!DMU_OT_IS_VALID(dnode->bonus_type)) {
			return false;
		}

		if (dnode->bonus_len == 0) {
			return false;
		}
	}

	if (dnode->bonus_type) {
		if (!DMU_OT_IS_VALID(dnode->bonus_type)) {
			return false;
		}

		if (dnode->bonus_len == 0) {
			return false;
		}	
	}
	if (dnode->bonus_len) {
		if (dnode->bonus_type == DMU_OT_NONE) {
			return false;
		}
		if (!DMU_OT_IS_VALID(dnode->bonus_type)) {
			return false;
		}
	}


	if (dnode->bonus_len > DN_MAX_BONUSLEN) return false;	

	if (dnode->pad2[0] != 0x00) return false;
	if (dnode->pad2[1] != 0x00) return false;
	if (dnode->pad2[2] != 0x00) return false;
	if (dnode->pad2[3] != 0x00) return false;

	if (dnode->pad3[0] != 0x00) return false;
	if (dnode->pad3[1] != 0x00) return false;
	if (dnode->pad3[2] != 0x00) return false;
	if (dnode->pad3[3] != 0x00) return false;

	return true;
}

void zfs_dnode_recovery(void)
{
	//std::string file_name = "D:\\zfs\\zfs-pool-flat.vmdk";
	std::string file_name = "\\\\.\\PhysicalDrive0";
	W32Lib::FileEx io;
	if (io.Open(file_name.c_str())) {

		size_t buff_size = 256* SECTOR_SIZE;
		std::vector<char> buffer(buff_size, 0x00);

		ULONGLONG offset = 0 * 512;
		ULONGLONG dnode_offset;
		size_t readed = 0;
		dnode_phys_t *dnode = nullptr;

		io.SetPointer(offset);

		while (readed = io.Read(buffer.data(), buffer.size())) {
			
			size_t sectors = readed >> SECTOR_SIZE_SHIFT;

			for (size_t i = 0; i < sectors; i++) {			
				dnode = (dnode_phys_t *)&buffer[i*SECTOR_SIZE];
				if (IsValidDnode(dnode)) {				
					dnode_offset = offset + i*SECTOR_SIZE;
					if (dnode->type == DMU_OT_PLAIN_FILE_CONTENTS || dnode->type == DMU_OT_ZVOL) {
					
						int x = 0;
						x++;
					
					}				
				}			
			}
			offset += readed;		
		}
	}
}


void zfs_test(void)
{
	W32Lib::FileEx io;
	//std::string volume_file_name = "D:\\zfs\\zfs-pool-flat.vmdk";
	std::string volume_file_name = "\\\\.\\PhysicalDrive0";
	if (!io.Open(volume_file_name.c_str())) {
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

	//vdev_sector_size = 1 << 0x0C;


	MetaObjecSet mos = { 0 };
	ReadMOS(io, ub->rootbp, mos);

	Dataset root_dataset = { 0 };
	ReadDataset(io, mos.objset, mos.root_dataset_obj, root_dataset);


	bool res = false;
	dnode_phys *dnode = nullptr;
	for (size_t i = 1; i < root_dataset.objset.size(); i++) {
		dnode = &root_dataset.objset[i];
		res = IsValidDnode(dnode);
		if (!res) {
			int x = 0;
			x++;
		}	
	}
	

	int object_id = 0x0f;

	std::vector<char> file_data;
	res = ReadObjectData(io, root_dataset.objset[object_id], file_data);

	res = IsValidDnode(&root_dataset.objset[object_id]);

	//W32Lib::FileEx out("f:\\bonus.bin");
	//if (out.Create()) {
	//	out.Write(root_dataset.objset[object_id].bonus, root_dataset.objset[object_id].bonus_len);
	//}

	//std::vector<char> zap_buff;


	int x = 0;
}

bool IsHole(const blkptr_t &bp)
{
	uint64_t *word = (uint64_t *)&bp.dva[0];
	if (!bp.props.embedded) {
		if ((word[0] == 0) && (word[1] == 0)) {
			return true;
		}
	}
	return false;
}

bool ReadBlock(W32Lib::FileEx &io, blkptr_t &blkptr, std::vector<char> &buffer)
{
	int result = 0;
	size_t decompressed_data_size = 0;
	size_t origin_size = buffer.size();
	size_t readed_size = 0;
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

		readed_size = props->physical_size;

	} else {

		//
		// TODO: Validating dva, and if need read as HOLE.
		//

		size_t data_size = 0;

		if (IsHole(blkptr)) {
			data_size = (blkptr.props.logical_size + 1) * vdev_sector_size;
			buffer.resize(origin_size + data_size);
			std::memset(&buffer[origin_size], 0x00, data_size);		
			return true;
		} else {
			data_size = blkptr.dva[0].alloc_size * vdev_sector_size;

			readed_size = data_size;

			if (blkptr.props.compression == ZIO_COMPRESS_OFF) {
				buffer.resize(origin_size + data_size);
				dst = &buffer[origin_size];
			}
			else {
				compressed_data.resize(data_size);
				dst = compressed_data.data();

				decompressed_data_size = (blkptr.props.logical_size + 1) * vdev_sector_size;
				buffer.resize(origin_size + decompressed_data_size);
			}

			if (!io.SetPointer(VDEV_OFFSET + VDEV_LABEL_START_SIZE + blkptr.dva[0].offset * vdev_sector_size)) {
				return false;
			}

			if (!io.Read(dst, data_size)) {
				return false;
			}		
		}
	}

	zio_cksum_t calculated_chksum = { 0 };

	switch (blkptr.props.checksum) {
	case ZIO_CHECKSUM_OFF:
		break;

	case ZIO_CHECKSUM_ON:
	case ZIO_CHECKSUM_ZILOG:
	case ZIO_CHECKSUM_FLETCHER_2:
		fletcher_2_native(dst, readed_size, &calculated_chksum);
		break;

	case ZIO_CHECKSUM_ZILOG2:
	case ZIO_CHECKSUM_FLETCHER_4:
		fletcher_4_native(dst, readed_size, &calculated_chksum);
		break;

	case ZIO_CHECKSUM_LABEL:
	case ZIO_CHECKSUM_GANG_HEADER:
	case ZIO_CHECKSUM_SHA256:
		zio_checksum_SHA256(dst, readed_size, &calculated_chksum);
		break;

	default:
		break;	
	}

	bool chksum_ok = ChecksumEqual(blkptr.checksum, calculated_chksum);

	switch (blkptr.props.compression) {
	
	case ZIO_COMPRESS_OFF:
		return true;

	case ZIO_COMPRESS_LZJB:
		result = lzjb_decompress(compressed_data.data(), &buffer[origin_size], compressed_data.size(), decompressed_data_size, 0);
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

bool ReadDataBlock(W32Lib::FileEx &io, dnode_phys_t &dnode, uint64_t block_num, std::vector<char> &buffer)
{
	assert(dnode.nlevels);

	size_t ptr_per_block = (1 << dnode.ind_blk_shift) / sizeof(blkptr_t);
	size_t level = dnode.nlevels - 1;
	size_t blkptr_idx = 0;
	//blkptr_t blkptr = {0};
	//uint64_t bnum = block_num;

	std::vector<char> tmp;
	tmp.reserve(dnode.data_blk_sz_sec * SPA_MINBLOCKSIZE);

	uint64_t blocks_per_ptr = std::pow(ptr_per_block, level);
	uint64_t blocks_per_level = blocks_per_ptr * dnode.nblk_ptr;

	assert(block_num < blocks_per_level);
	assert(block_num <= dnode.max_blk_id);

	blkptr_t blkptr = dnode.blk_ptr[block_num / blocks_per_ptr];
	uint64_t bnum = block_num % blocks_per_ptr;

	while (level) {

		assert(blkptr.props.level == level);

		tmp.clear();
		ReadBlock(io, blkptr, tmp);

		if (level == 1) {
			assert(bnum < ptr_per_block);
			blkptr_idx = bnum;
		} else {
			blocks_per_ptr = std::pow(ptr_per_block, level);
			blkptr_idx = bnum / blocks_per_ptr;
			bnum = bnum % blocks_per_ptr;		
		}

		blkptr = ((blkptr_t *)tmp.data())[blkptr_idx];

		level--;
	}

	assert(blkptr.props.level == 0);

	return ReadBlock(io, blkptr, buffer);
}

bool ReadObjectData(W32Lib::FileEx &io, dnode_phys_t &dnode, std::vector<char> &buffer)
{
	buffer.clear();
	for (int i = 0; i <= dnode.max_blk_id; i++) {
		if (!ReadDataBlock(io, dnode, i, buffer)) {
			return false;
		}
	}
	return true;
}

bool ReadDataset(W32Lib::FileEx &io, std::vector<dnode_phys_t> objset, uint64_t os_object, Dataset &dataset)
{
	assert(os_object < objset.size());

	dnode_phys_t *dataset_dnode = &objset[os_object];
	assert(dataset_dnode->type == DMU_OT_DSL_DIR);
	assert(dataset_dnode->bonus_type == DMU_OT_DSL_DIR);
	dsl_dir_phys_t *dir_phys = (dsl_dir_phys_t *)dataset_dnode->bonus;


	std::vector<char> buff;
	for (int i = 0; i <= objset[dir_phys->dd_child_dir_zapobj].max_blk_id; i++) {
		if (!ReadDataBlock(io, objset[dir_phys->dd_child_dir_zapobj], i, buff)) {
			return false;
		}
	}

	std::map<std::string, uint64_t> &child_dir = dataset.child_dir;
	TraversingMicroZapEntries(buff,
		[&child_dir](const uint64_t &value, const char* name) {
		child_dir.emplace(std::string(name), value);
		return true;
	});


	//
	// Read properties ZAP Object
	//
	dnode_phys_t &props_dnode = objset[dir_phys->dd_props_zapobj];
	assert(props_dnode.type == DMU_OT_DSL_PROPS);

	buff.clear();
	for (int i = 0; i <= props_dnode.max_blk_id; i++) {
		if (!ReadDataBlock(io, props_dnode, i, buff)) {
			return false;
		}
	}

	std::map<std::string, uint64_t> props;
	TraversingMicroZapEntries(buff,
		[&props](const uint64_t &value, const char* name) {
		props.emplace(std::string(name), value);
		return true;
	});


	//
	// Read Dataset`s objset
	//
	dnode_phys_t &active_dataset_dnode = objset[dir_phys->dd_head_dataset_obj];
	assert(active_dataset_dnode.bonus_type == DMU_OT_DSL_DATASET);
	dsl_dataset_phys_t *dataset_phys = (dsl_dataset_phys_t *)active_dataset_dnode.bonus;


	buff.clear();
	ReadBlock(io, dataset_phys->ds_bp, buff);

	objset_phys_t ds_objset = *(objset_phys_t *)buff.data();

	buff.clear();
	for (int i = 0; i <= ds_objset.os_meta_dnode.max_blk_id; i++) {
		if (!ReadDataBlock(io, ds_objset.os_meta_dnode, i, buff)) {
			return false;
		}
	}

	dataset.objset.resize(buff.size() / sizeof(dnode_phys_t));
	std::memcpy(dataset.objset.data(), buff.data(), dataset.objset.size() * sizeof(dnode_phys_t));

	//
	// Read "master node"
	//
	dnode_phys_t &master_node_dnode = dataset.objset[1];
	assert(master_node_dnode.type == DMU_OT_MASTER_NODE);

	buff.clear();
	for (int i = 0; i <= master_node_dnode.max_blk_id; i++) {
		if (!ReadDataBlock(io, master_node_dnode, i, buff)) {
			return false;
		}
	}

	std::map<std::string, uint64_t> master_node_attr;
	TraversingMicroZapEntries(buff,
		[&master_node_attr](const uint64_t &value, const char* name) {
		master_node_attr.emplace(std::string(name), value);
		return false;
	});

	//
	// Read "ROOT"
	//
	dnode_phys_t root_dnode = dataset.objset[master_node_attr["ROOT"]];
	assert(root_dnode.type == DMU_OT_DIRECTORY_CONTENTS);

	buff.clear();
	for (int i = 0; i <= root_dnode.max_blk_id; i++) {
		if (!ReadDataBlock(io, root_dnode, i, buff)) {
			return false;
		}
	}


	//W32Lib::FileEx fat_zap;
	//if (fat_zap.Create("D:\\zfs\\fat_zat.bin")) {
	//	fat_zap.Write(buff.data(), buff.size());
	//}


	//std::map<std::string, uint64_t> root_dir;
	TraversingFatZapEntries(io, root_dnode, 
		[&dataset](const uint64_t &value, const char* name) {
		dataset.root_dir.emplace(std::string(name), value);
		return true;
	});


	return false;
}

bool ReadMOS(W32Lib::FileEx & io, blkptr_t &blkptr, MetaObjecSet &mos)
{
	assert(blkptr.props.type == DMU_OT_OBJSET);

	std::vector<char> buff;
	if (!ReadBlock(io, blkptr, buff)) {
		return false;
	}
	
	if (((objset_phys_t *)buff.data())->os_type != DMU_OST_META) {
		return false;
	}

	mos.objset_phys = *(objset_phys_t *)buff.data();

	buff.clear();
	for (int i = 0; i <= mos.objset_phys.os_meta_dnode.max_blk_id; i++) {
		if (!ReadDataBlock(io, mos.objset_phys.os_meta_dnode, i, buff)) {
			return false;
		}
	}

	mos.objset.resize(buff.size()/sizeof(dnode_phys_t));
	std::memcpy(mos.objset.data(), buff.data(), mos.objset.size() * sizeof(dnode_phys_t));

	dnode_phys_t &obj_dir_dnode = mos.objset[DMU_POOL_DIRECTORY_OBJECT];
	if (obj_dir_dnode.type != DMU_OT_OBJECT_DIRECTORY) {
		return false;
	}

	//buff.clear();
	//for (int i = 0; i <= obj_dir_dnode.max_blk_id; i++) {
	//	if (!ReadDataBlock(io, obj_dir_dnode, i, buff)) {
	//		return false;
	//	}
	//}

	//W32Lib::FileEx fat_zap;
	//if (fat_zap.Create("D:\\zfs\\fat_zap.bin")) {
	//	fat_zap.Write(buff.data(), buff.size());
	//}

	//std::map<std::string, uint64_t> &object_dir = mos.object_dir;
	//TraversingMicroZapEntries(buff,
	//	[&mos](const uint64_t &value, const char* name) {
	//	if (std::strcmp(name, DMU_POOL_ROOT_DATASET) == 0) {
	//		mos.root_dataset_obj = value;
	//	}
	//	mos.object_dir.emplace(std::string(name), value);
	//	return true;
	//});

	TraversingZapObject(io, obj_dir_dnode,
		[&mos](const uint64_t &value, const char* name) {
		if (std::strcmp(name, DMU_POOL_ROOT_DATASET) == 0) {
			mos.root_dataset_obj = value;
		}
		mos.object_dir.emplace(std::string(name), value);
		return true;
	});


	return true;
}

bool zfs_blkptr_verify(const blkptr_t &bp)
{
	bool result = true;
	std::string description = "blkptr has: ";

	if (!DMU_OT_IS_VALID(bp.props.type)) {
		description += " Invalid DMU Object TYPE;";
		result = false;
	}

	if (bp.props.compression >= ZIO_COMPRESS_FUNCTIONS ) {
		description += " Invalid COMPRESS;";
		result = false;
	}

	if (bp.props.logical_size > SPA_MAXBLOCKSIZE) {
		description += "Invalid LSIZE;";
	}

	if (bp.props.physical_size > SPA_MAXBLOCKSIZE) {
		description += "Invalid PSIZE;";
		result = false;
	}

	if (bp.props.embedded) {
		blk_props_emb_t *props = (blk_props_emb_t *)&bp.props;
		if (props->etype > NUM_BP_EMBEDDED_TYPES) {
			description += " Invalid EMBEDDED TYPE;";
			result = false;
		}
		if (props->physical_size > 6 * 8 + 3 * 8 + 5 * 8) {
			result = false;
		}
	} else {
		if (bp.props.checksum >= ZIO_CHECKSUM_FUNCTIONS) {
			description += " Invalid CHECKSUM;";
			result = false;
		}
	}

	/*
	* Pool-specific checks.
	*
	* Note: it would be nice to verify that the blk_birth and
	* BP_PHYSICAL_BIRTH() are not too large.  However, spa_freeze()
	* allows the birth time of log blocks (and dmu_sync()-ed blocks
	* that are in the log) to be arbitrarily large.
	*/

	//for (int i = 0; i < BP_GET_NDVAS(bp); i++) {

	//	uint64_t vdevid = DVA_GET_VDEV(&bp->blk_dva[i]);
	//	vdev_t *vd;
	//	uint64_t offset, asize;
	//	if (vdevid >= spa->spa_root_vdev->vdev_children) {
	//		zfs_panic_recover("blkptr at %p DVA %u has invalid "
	//			"VDEV %llu",
	//			bp, i, (longlong_t)vdevid);
	//	}
	//	vd = spa->spa_root_vdev->vdev_child[vdevid];
	//	if (vd == NULL) {
	//		zfs_panic_recover("blkptr at %p DVA %u has invalid "
	//			"VDEV %llu",
	//			bp, i, (longlong_t)vdevid);
	//	}
	//	if (vd->vdev_ops == &vdev_hole_ops) {
	//		zfs_panic_recover("blkptr at %p DVA %u has hole "
	//			"VDEV %llu",
	//			bp, i, (longlong_t)vdevid);

	//	}
	//	if (vd->vdev_ops == &vdev_missing_ops) {
	//		/*
	//		* "missing" vdevs are valid during import, but we
	//		* don't have their detailed info (e.g. asize), so
	//		* we can't perform any more checks on them.
	//		*/
	//		continue;
	//	}

	//	offset = DVA_GET_OFFSET(&bp->blk_dva[i]);
	//	asize = DVA_GET_ASIZE(&bp->blk_dva[i]);
	//	if (BP_IS_GANG(bp))
	//		asize = vdev_psize_to_asize(vd, SPA_GANGBLOCKSIZE);
	//	if (offset + asize > vd->vdev_asize) {
	//		zfs_panic_recover("blkptr at %p DVA %u has invalid "
	//			"OFFSET %llu",
	//			bp, i, (longlong_t)offset);
	//	}
	//}

	return result;
}

int ByteSizeToShiftSize(uint64_t byte_size)
{
	if (byte_size) {
		for (size_t shift = 0; shift < sizeof(uint64_t) * 8; shift++) {
			if (byte_size & (uint64_t)1) { return shift; }
			byte_size >>= 1;
		}
	}
	return -1;
}

bool TraversingZapObject(W32Lib::FileEx &io, dnode_phys_t &dnode, std::function<bool(const uint64_t&, const char*)> callback)
{
	std::vector<char> buffer;
	buffer.reserve(dnode.data_blk_sz_sec*SPA_MINBLOCKSIZE);

	if (!ReadDataBlock(io, dnode, 0, buffer)) {
		return false;
	}

	zap_phys *zap_hdr = (zap_phys *)buffer.data();

	if (zap_hdr->block_type == ZBT_MICRO) {	
		buffer.clear();
		for (int i = 0; i <= dnode.max_blk_id; i++) {
			if (!ReadDataBlock(io, dnode, i, buffer)) {
				return false;
			}
		}
		return TraversingMicroZapEntries(buffer, callback);
	} else if ((zap_hdr->block_type == ZBT_HEADER) && (zap_hdr->magic == ZAP_MAGIC)) {
		return TraversingFatZapEntries(io, dnode, callback);	
	}

	return false;
}

bool TraversingFatZapEntries(W32Lib::FileEx &io, dnode_phys_t &dnode, std::function<bool(const uint64_t&, const char*)> callback)
{
	size_t block_size = dnode.data_blk_sz_sec * SPA_MINBLOCKSIZE;
	std::vector<char> buffer;
	buffer.reserve(block_size);

	if (!ReadDataBlock(io, dnode, 0, buffer)) { return false; }

	zap_phys_t zap_phys = *(zap_phys_t *)buffer.data();

	if (zap_phys.block_type != ZBT_HEADER) { return false;	}
	if (zap_phys.magic != ZAP_MAGIC) { return false; }

	//
	// На данный момент реализовна поддержка только встроенной таблици указателей
	//

	assert(zap_phys.ptrtbl.start_blk == 0);
	assert(zap_phys.ptrtbl.num_blks == 0);

	size_t block_size_shift = ByteSizeToShiftSize(block_size);
	size_t num_hash_entries = ZAP_LEAF_HASH_NUMENTRIES(block_size_shift);
	size_t num_chunks = ZAP_LEAF_NUMCHUNKS(block_size_shift);

	std::vector<char> leaf_buffer;
	leaf_buffer.reserve(block_size);

	for (int block = 1; block <= dnode.max_blk_id; block++) {
	
		leaf_buffer.clear();
		if (!ReadDataBlock(io, dnode, block, leaf_buffer)) { return false; }
	
		zap_leaf_phys *leaf = (zap_leaf_phys *)leaf_buffer.data();

		if ((leaf->hdr.block_type != ZBT_LEAF) || (leaf->hdr.magic != ZAP_LEAF_MAGIC)) {
			continue;
		}

		zap_leaf_chunk *chunks = (zap_leaf_chunk *)(leaf_buffer.data() + 2 * ZAP_LEAF_CHUNKSIZE + 2 * num_hash_entries);

		for (int i = 0; i < num_hash_entries; i++) {

			if ((leaf->hash[i] != CHAIN_END) && (leaf->hash[i] < num_chunks)) {
			
				zap_leaf_chunk::zap_leaf_entry *entry = (zap_leaf_chunk::zap_leaf_entry *)&chunks[leaf->hash[i]];
				while (entry != nullptr) {
					if (entry->type != ZAP_CHUNK_ENTRY) {
						continue;
					}
					if (entry->name_length > ZAP_MAXNAMELEN) {
						continue;
					}
					if ((entry->value_intlen != 1) && (entry->value_intlen != 2) && (entry->value_intlen != 4) && (entry->value_intlen != 8)) {
						continue;
					}
					if (entry->value_intlen * entry->value_numints > ZAP_MAXVALUELEN) {
						continue;
					}

					std::string name = "";
					name.reserve(ZAP_MAXNAMELEN);
					size_t name_len = entry->name_length;
					if (entry->name_chunk >= num_chunks) {
						continue;
					}

					zap_leaf_chunk::zap_leaf_array *chunk_array = (zap_leaf_chunk::zap_leaf_array *)&chunks[entry->name_chunk];

					while (name_len) {
						if (chunk_array->type != ZAP_CHUNK_ARRAY) {
							break;
						}
						if (name_len > ZAP_LEAF_ARRAY_BYTES) {
							name += std::string((const char *)chunk_array->array, ZAP_LEAF_ARRAY_BYTES);
							name_len -= ZAP_LEAF_ARRAY_BYTES;
							if ((chunk_array->next == CHAIN_END) || (chunk_array->next > num_chunks)) {
								break;
							}
							chunk_array = (zap_leaf_chunk::zap_leaf_array *)&chunks[chunk_array->next];
						} else {
							name += std::string((const char *)chunk_array->array, name_len);
							break;
						}
					}

					//
					// Судя с коментов в сорцах, value может быть массивом, но как и зачем непанятна.
					// Пока реализую поддержку только одного значения. 
					//
					assert(entry->value_numints == 1);

					if (entry->value_chunk >= num_chunks) {
						break;
					}

					chunk_array = (zap_leaf_chunk::zap_leaf_array *)&chunks[entry->value_chunk];
					if (chunk_array->type != ZAP_CHUNK_ARRAY) {
						break;
					}

					uint64_t value = 0;
					switch (entry->value_intlen) {
					case 1:
						value = *(uint8_t *)chunk_array->array;
						break;
					case 2:
						value = _byteswap_ushort(*(uint16_t *)chunk_array->array);
						break;
					case 4:
						value = _byteswap_ulong(*(uint32_t *)chunk_array->array);
						break;
					case 8:
						value = _byteswap_uint64(*(uint64_t *)chunk_array->array);
						break;
					default:
						break;
					}

					if (!callback(value, name.c_str())) {
						return true;
					}
										
					if ((entry->next != CHAIN_END) && (entry->next < num_chunks)) {
						entry = (zap_leaf_chunk::zap_leaf_entry *)&chunks[entry->next];
					} else {
						entry = nullptr;
					}
				}
			}		
		}
	}
	return true;
}
