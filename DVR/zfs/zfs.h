#ifndef _ZFS_H
#define _ZFS_H

#include "zfs_type.h"


#pragma pack(push, 1)

/*
* We currently support block sizes from 512 bytes to 16MB.
* The benefits of larger blocks, and thus larger IO, need to be weighed
* against the cost of COWing a giant block to modify one byte, and the
* large latency of reading or writing a large block.
*
* Note that although blocks up to 16MB are supported, the recordsize
* property can not be set larger than zfs_max_recordsize (default 1MB).
* See the comment near zfs_max_recordsize in dsl_dataset.c for details.
*
* Note that although the LSIZE field of the blkptr_t can store sizes up
* to 32MB, the dnode's dn_datablkszsec can only store sizes up to
* 32MB - 512 bytes.  Therefore, we limit SPA_MAXBLOCKSIZE to 16MB.
*/
#define	SPA_MINBLOCKSHIFT				9
#define	SPA_OLD_MAXBLOCKSHIFT			17
#define	SPA_MAXBLOCKSHIFT				24
#define	SPA_MINBLOCKSIZE				(1ULL << SPA_MINBLOCKSHIFT)
#define	SPA_OLD_MAXBLOCKSIZE			(1ULL << SPA_OLD_MAXBLOCKSHIFT)
#define	SPA_MAXBLOCKSIZE				(1ULL << SPA_MAXBLOCKSHIFT)

enum zio_checksum {
	ZIO_CHECKSUM_INHERIT = 0,
	ZIO_CHECKSUM_ON,
	ZIO_CHECKSUM_OFF,
	ZIO_CHECKSUM_LABEL,
	ZIO_CHECKSUM_GANG_HEADER,
	ZIO_CHECKSUM_ZILOG,
	ZIO_CHECKSUM_FLETCHER_2,
	ZIO_CHECKSUM_FLETCHER_4,
	ZIO_CHECKSUM_SHA256,
	ZIO_CHECKSUM_ZILOG2,
	ZIO_CHECKSUM_FUNCTIONS
};

enum zio_compress {
	ZIO_COMPRESS_INHERIT = 0,
	ZIO_COMPRESS_ON,
	ZIO_COMPRESS_OFF,
	ZIO_COMPRESS_LZJB,
	ZIO_COMPRESS_EMPTY,
	ZIO_COMPRESS_GZIP_1,
	ZIO_COMPRESS_GZIP_2,
	ZIO_COMPRESS_GZIP_3,
	ZIO_COMPRESS_GZIP_4,
	ZIO_COMPRESS_GZIP_5,
	ZIO_COMPRESS_GZIP_6,
	ZIO_COMPRESS_GZIP_7,
	ZIO_COMPRESS_GZIP_8,
	ZIO_COMPRESS_GZIP_9,
	ZIO_COMPRESS_ZLE,
	ZIO_COMPRESS_LZ4,
	ZIO_COMPRESS_FUNCTIONS
};

typedef struct zio_cksum {
	uint64_t	word[4];
} zio_cksum_t;

typedef struct dva {
	uint64_t	alloc_size:24;				/* allocated size in sectors (including RAID-Z parity and gang block headers) */
	uint64_t	grid:8;						/* RAID-Z layout information (reserved for future use) */
	uint64_t	vdev_id:32;					/* virtual device ID */
	uint64_t	offset:63;					/* offset in sectors (512 bytes) */
	uint64_t	gang_flag:1;				/* gang block indicator */
} dva_t;

typedef struct blk_props {
	uint64_t	logical_size:16;			/* logical size */
	uint64_t	physical_size:16;			/* physical size (after compression) */
	uint64_t	compression:7;				/* compression function */
	uint64_t	embedded:1;					/* blkptr_t contains embedded data (see below) blk_props_emb_t */
	uint64_t	checksum:8;					/* checksum function */
	uint64_t	type:8;						/* DMU object type */
	uint64_t	level:5;					/* level of indirection */
	uint64_t	encryption:1;				/* encryption (on version 30, which is not supported) */
	uint64_t	dedup:1;					/* deduplication */
	uint64_t	byteorder:1;				/* byteorder (endianness)*/
} blk_props_t;

typedef struct blk_props_emb {
	uint64_t	logical_size:25;			/* logical size */
	uint64_t	physical_size:7;			/* physical size (after compression) */
	uint64_t	compression:7;				/* compression function */
	uint64_t	embedded:1;					/* set to one */
	uint64_t	etype:8;					/* how to interpret embedded data (BP_EMBEDDED_TYPE_*) */
	uint64_t	type:8;						/* DMU object type */
	uint64_t	level:5;					/* level of indirection */
	uint64_t	encryption:1;				/* encryption (on version 30, which is not supported) */
	uint64_t	dedup:1;					/* deduplication */
	uint64_t	byteorder:1;				/* byteorder (endianness)*/
} blk_props_emb_t;

#define	SPA_DVAS_PER_BP		3				/* Number of DVAs in a bp */
#define	SPA_BLKPTRSHIFT		7				/* blkptr_t is 128 bytes */

typedef struct blkptr {
	dva_t		dva[SPA_DVAS_PER_BP];		/* Data Virtual Addresses */
	blk_props_t	props;						/* size, compression, type, etc */
	uint64_t	padding[2];					/* Extra space for the future */
	uint64_t	phys_birth;					/* txg of block allocation; zero if same as logical birth txg */
	uint64_t	log_birth;					/* transaction group in which the block was logically born */
	uint64_t	fill;						/* fill count */
	zio_cksum_t	checksum;					/* 256-bit checksum */
} blkptr_t;

#define	UBERBLOCK_MAGIC		0x00bab10c		/* oo-ba-bloc! */
#define	UBERBLOCK_SHIFT		10				/* up to 1K	*/

typedef struct uberblock {
	uint64_t	magic;						/* UBERBLOCK_MAGIC */
	uint64_t	version;					/* SPA_VERSION */
	uint64_t	txg;						/* txg of last sync */
	uint64_t	guid_sum;					/* sum of all vdev guids */
	uint64_t	timestamp;					/* UTC time of last sync */
	blkptr_t	rootbp;						/* MOS objset_phys_t */

	/* highest SPA_VERSION supported by software that wrote this txg */
	uint64_t	software_version;
} uberblock_t;

#define	DNODE_SHIFT			9				/* 512 bytes */
#define	DNODE_SIZE			(1 << DNODE_SHIFT)
#define	DNODE_CORE_SIZE		64				/* 64 bytes for dnode sans blkptrs */
#define	DN_MAX_BONUSLEN		(DNODE_SIZE - DNODE_CORE_SIZE - (1 << SPA_BLKPTRSHIFT))

typedef struct dnode_phys {
	uint8_t		type;						/* dmu_object_type_t */
	uint8_t		ind_blk_shift;				/* ln2(indirect block size) */
	uint8_t		nlevels;					/* 1=dn_blkptr->data blocks */
	uint8_t		nblk_ptr;					/* length of dn_blkptr */
	uint8_t		bonus_type;					/* type of data in bonus buffer */
	uint8_t		checksum;					/* ZIO_CHECKSUM type */
	uint8_t		compress;					/* ZIO_COMPRESS type */
	uint8_t		flags;						/* DNODE_FLAG_* */
	uint16_t	data_blk_sz_sec;			/* data block size in 512b sectors */
	uint16_t	bonus_len;					/* length of dn_bonus */
	uint8_t		pad2[4];

	/* accounting is protected by dn_dirty_mtx */
	uint64_t	max_blk_id;					/* largest allocated block ID */
	uint64_t	used;						/* bytes (or sectors) of disk space */
	uint64_t	pad3[4];

	/*
	 * The tail region is 448 bytes, and there are three ways to
	 * look at it.
	 *
	 * 0       64      128     192     256     320     384     448 (offset)
	 * +---------------+---------------+---------------+-------+
	 * | dn_blkptr[0]  | dn_blkptr[1]  | dn_blkptr[2]  | /     |
	 * +---------------+---------------+---------------+-------+
	 * | dn_blkptr[0]  | dn_bonus[0..319]                      |
	 * +---------------+-----------------------+---------------+
	 * | dn_blkptr[0]  | /                     | dn_spill      |
	 * +---------------+-----------------------+---------------+
	 */
	union {
		blkptr_t		blk_ptr[1+DN_MAX_BONUSLEN/sizeof(blkptr_t)];
		struct {
			blkptr_t	ignore1;
			uint8_t		bonus[DN_MAX_BONUSLEN];
		};
		struct {
			blkptr_t	ignore2;
			uint8_t		ignore3[DN_MAX_BONUSLEN-sizeof(blkptr_t)];
			blkptr_t	spill;
		};
	};
} dnode_phys_t;


/*
* Intent log header - this on disk structure holds fields to manage
* the log.  All fields are 64 bit to easily handle cross architectures.
*/
typedef struct zil_header {
	uint64_t zh_claim_txg;			/* txg in which log blocks were claimed */
	uint64_t zh_replay_seq;			/* highest replayed sequence number */
	blkptr_t zh_log;				/* log chain */
	uint64_t zh_claim_blk_seq;		/* highest claimed block sequence number */
	uint64_t zh_flags;				/* header flags */
	uint64_t zh_claim_lr_seq;		/* highest claimed lr sequence number */
	uint64_t zh_pad[3];
} zil_header_t;

#define	OBJSET_PHYS_SIZE 2048
#define	OBJSET_OLD_PHYS_SIZE 1024

#define	OBJSET_BUF_HAS_USERUSED(buf) \
	(arc_buf_size(buf) > OBJSET_OLD_PHYS_SIZE)

#define	OBJSET_FLAG_USERACCOUNTING_COMPLETE	(1ULL<<0)

typedef struct objset_phys {
	dnode_phys_t os_meta_dnode;
	zil_header_t os_zil_header;
	uint64_t os_type;
	uint64_t os_flags;
	char os_pad[OBJSET_PHYS_SIZE - sizeof(dnode_phys_t) * 3 -
		sizeof(zil_header_t) - sizeof(uint64_t) * 2];
	dnode_phys_t os_userused_dnode;
	dnode_phys_t os_groupused_dnode;
} objset_phys_t;

/*
* Embedded checksum
*/
#define	ZEC_MAGIC	0x210da7ab10c7a11ULL		// zio-data-block-tail

typedef struct zio_eck {
	uint64_t	zec_magic;			/* for validation, endianness	*/
	zio_cksum_t	zec_cksum;			/* 256-bit checksum		*/
} zio_eck_t;

/*
* Gang block headers are self-checksumming and contain an array
* of block pointers.
*/
#define	SPA_GANGBLOCKSIZE	SPA_MINBLOCKSIZE
#define	SPA_GBH_NBLKPTRS	((SPA_GANGBLOCKSIZE - sizeof (zio_eck_t)) / sizeof (blkptr_t))
#define	SPA_GBH_FILLER		((SPA_GANGBLOCKSIZE - sizeof (zio_eck_t) - (SPA_GBH_NBLKPTRS * sizeof (blkptr_t))) / sizeof (uint64_t))

typedef struct zio_gbh {
	blkptr_t		zg_blkptr[SPA_GBH_NBLKPTRS];
	uint64_t		zg_filler[SPA_GBH_FILLER];
	zio_eck_t		zg_tail;
} zio_gbh_phys_t;

#pragma pack(pop)

#endif // _ZFS_H