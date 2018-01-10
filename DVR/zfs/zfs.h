#ifndef _ZFS_H
#define _ZFS_H

#include <assert.h>
#include "zfs_type.h"
#include "vdev_label.h"
#include "spa.h"


#pragma pack(push, 1)

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

#define	DN_MIN_INDBLKSHIFT	10	/* 1k */
#define	DN_MAX_INDBLKSHIFT	14	/* 16k */

/* Is dn_used in bytes?  if not, it's in multiples of SPA_MINBLOCKSIZE */
#define	DNODE_FLAG_USED_BYTES			(1<<0)
#define	DNODE_FLAG_USERUSED_ACCOUNTED	(1<<1)

/* Does dnode have a SA spill blkptr in bonus? */
#define	DNODE_FLAG_SPILL_BLKPTR			(1<<2)

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

/*
* The following are configuration names used in the nvlist describing a pool's
* configuration.
*/
#define	ZPOOL_CONFIG_VERSION			"version"
#define	ZPOOL_CONFIG_POOL_NAME			"name"
#define	ZPOOL_CONFIG_POOL_STATE			"state"
#define	ZPOOL_CONFIG_POOL_TXG			"txg"
#define	ZPOOL_CONFIG_POOL_GUID			"pool_guid"
#define	ZPOOL_CONFIG_CREATE_TXG			"create_txg"
#define	ZPOOL_CONFIG_TOP_GUID			"top_guid"
#define	ZPOOL_CONFIG_VDEV_TREE			"vdev_tree"
#define	ZPOOL_CONFIG_TYPE				"type"
#define	ZPOOL_CONFIG_CHILDREN			"children"
#define	ZPOOL_CONFIG_ID					"id"
#define	ZPOOL_CONFIG_GUID				"guid"
#define	ZPOOL_CONFIG_PATH				"path"
#define	ZPOOL_CONFIG_DEVID				"devid"
#define	ZPOOL_CONFIG_METASLAB_ARRAY		"metaslab_array"
#define	ZPOOL_CONFIG_METASLAB_SHIFT		"metaslab_shift"
#define	ZPOOL_CONFIG_ASHIFT				"ashift"
#define	ZPOOL_CONFIG_ASIZE				"asize"
#define	ZPOOL_CONFIG_DTL				"DTL"
#define	ZPOOL_CONFIG_SCAN_STATS			"scan_stats"	/* not stored on disk */
#define	ZPOOL_CONFIG_VDEV_STATS			"vdev_stats"	/* not stored on disk */
#define	ZPOOL_CONFIG_WHOLE_DISK			"whole_disk"
#define	ZPOOL_CONFIG_ERRCOUNT			"error_count"
#define	ZPOOL_CONFIG_NOT_PRESENT		"not_present"
#define	ZPOOL_CONFIG_SPARES				"spares"
#define	ZPOOL_CONFIG_IS_SPARE			"is_spare"
#define	ZPOOL_CONFIG_NPARITY			"nparity"
#define	ZPOOL_CONFIG_HOSTID				"hostid"
#define	ZPOOL_CONFIG_HOSTNAME			"hostname"
#define	ZPOOL_CONFIG_LOADED_TIME		"initial_load_time"
#define	ZPOOL_CONFIG_UNSPARE			"unspare"
#define	ZPOOL_CONFIG_PHYS_PATH			"phys_path"
#define	ZPOOL_CONFIG_IS_LOG				"is_log"
#define	ZPOOL_CONFIG_L2CACHE			"l2cache"
#define	ZPOOL_CONFIG_HOLE_ARRAY			"hole_array"
#define	ZPOOL_CONFIG_VDEV_CHILDREN		"vdev_children"
#define	ZPOOL_CONFIG_IS_HOLE			"is_hole"
#define	ZPOOL_CONFIG_DDT_HISTOGRAM		"ddt_histogram"
#define	ZPOOL_CONFIG_DDT_OBJ_STATS		"ddt_object_stats"
#define	ZPOOL_CONFIG_DDT_STATS			"ddt_stats"
#define	ZPOOL_CONFIG_SPLIT				"splitcfg"
#define	ZPOOL_CONFIG_ORIG_GUID			"orig_guid"
#define	ZPOOL_CONFIG_SPLIT_GUID			"split_guid"
#define	ZPOOL_CONFIG_SPLIT_LIST			"guid_list"
#define	ZPOOL_CONFIG_REMOVING			"removing"
#define	ZPOOL_CONFIG_RESILVER_TXG		"resilver_txg"
#define	ZPOOL_CONFIG_COMMENT			"comment"
#define	ZPOOL_CONFIG_SUSPENDED			"suspended"		/* not stored on disk */
#define	ZPOOL_CONFIG_TIMESTAMP			"timestamp"		/* not stored on disk */
#define	ZPOOL_CONFIG_BOOTFS				"bootfs"		/* not stored on disk */
#define	ZPOOL_CONFIG_MISSING_DEVICES	"missing_vdevs"	/* not stored on disk */
#define	ZPOOL_CONFIG_LOAD_INFO			"load_info"		/* not stored on disk */
#define	ZPOOL_CONFIG_REWIND_INFO		"rewind_info"	/* not stored on disk */
#define	ZPOOL_CONFIG_UNSUP_FEAT			"unsup_feat"	/* not stored on disk */
#define	ZPOOL_CONFIG_ENABLED_FEAT		"enabled_feat"	/* not stored on disk */
#define	ZPOOL_CONFIG_CAN_RDONLY			"can_rdonly"	/* not stored on disk */
#define	ZPOOL_CONFIG_FEATURES_FOR_READ	"features_for_read"
#define	ZPOOL_CONFIG_FEATURE_STATS		"feature_stats"	/* not stored on disk */
#define	ZPOOL_CONFIG_ERRATA				"errata"		/* not stored on disk */

/*
* The persistent vdev state is stored as separate values rather than a single
* 'vdev_state' entry.  This is because a device can be in multiple states, such
* as offline and degraded.
*/
#define	ZPOOL_CONFIG_OFFLINE			"offline"
#define	ZPOOL_CONFIG_FAULTED			"faulted"
#define	ZPOOL_CONFIG_DEGRADED			"degraded"
#define	ZPOOL_CONFIG_REMOVED			"removed"
#define	ZPOOL_CONFIG_FRU				"fru"
#define	ZPOOL_CONFIG_AUX_STATE			"aux_state"

#define	VDEV_TYPE_ROOT					"root"
#define	VDEV_TYPE_MIRROR				"mirror"
#define	VDEV_TYPE_REPLACING				"replacing"
#define	VDEV_TYPE_RAIDZ					"raidz"
#define	VDEV_TYPE_DISK					"disk"
#define	VDEV_TYPE_FILE					"file"
#define	VDEV_TYPE_MISSING				"missing"
#define	VDEV_TYPE_HOLE					"hole"
#define	VDEV_TYPE_SPARE					"spare"
#define	VDEV_TYPE_LOG					"log"
#define	VDEV_TYPE_L2CACHE				"l2cache"

#endif // _ZFS_H