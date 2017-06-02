#include "zfs_test.h"
#include "dmu.h"
#include <memory>
#include <string>
#include "sha256.h"
#include "lz4.h"

#define VDEV_OFFSET						2048*512
#define VDEV_LABEL_NVLIST_OFFSET		16*1024

bool ReadBlock(W32Lib::FileEx &io, blkptr_t &blkptr, std::vector<char> &buffer);

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

	zio_cksum_t old = label->vl_vdev_phys.vp_zbt.zec_cksum;
	zio_cksum_t chksum = {0};

	//memset(&label->vl_vdev_phys.vp_nvlist, 0x00, sizeof(label->vl_vdev_phys.vp_zbt.zec_cksum));

	zio_checksum_SHA256( &label->vl_vdev_phys.vp_nvlist, sizeof( label->vl_vdev_phys.vp_nvlist ), &chksum );


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
	// Read object set array
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

	for (int i = 0; i < dn_count; ++i) {
		
		dnode = (dnode_phys_t *)mos_buff.data() + i;
		dmu_object_type obj_type = (dmu_object_type)dnode->type;
		if (obj_type == 0x01) {
		
			std::vector<char> buff;

			bool res = ReadBlock(io, dnode->blk_ptr[0], buff);

			int x = 0;
		
		}

		if (obj_type == 19) {

			std::vector<char> buff;

			bool res = ReadBlock(io, dnode->blk_ptr[0], buff);

			int x = 0;

		}
		
	}


	int z = 0;
}

bool ReadBlock(W32Lib::FileEx &io, blkptr_t &blkptr, std::vector<char> &buffer)
{
	if (blkptr.props.embedded) {
		//
		// TODO:
		//
		assert(false);
	}

	if (!io.SetPointer(VDEV_OFFSET + VDEV_DATA_OFFSET + blkptr.dva[0].offset * 512)) {
		return false;
	}

	int result = 0;
	dnode_phys_t *dnode = nullptr;
	size_t origin_size = buffer.size();
	size_t block_size = blkptr.dva[0].alloc_size * 512;

	std::vector<char> compressed_data;
	void *dst = nullptr;

	if (blkptr.props.compression == ZIO_COMPRESS_OFF) {
		buffer.resize(origin_size + block_size);
		dst = &buffer[origin_size];
	} else {
		compressed_data.resize(block_size);
		dst = compressed_data.data();	
	}
	
	if (!io.Read(dst, block_size)) {
		return false;
	}

	switch (blkptr.props.compression) {
	
	case ZIO_COMPRESS_OFF:
		return true;

	case ZIO_COMPRESS_LZ4:

		block_size = (blkptr.props.logical_size + 1) * 512;
		buffer.resize(origin_size + block_size);
		result = lz4_decompress_zfs(compressed_data.data(), &buffer[origin_size], compressed_data.size(), block_size, 0);

		assert(result == 0);

		return true;
	
	default :
		assert(false);
		return false;	
	}


	return false;
}


