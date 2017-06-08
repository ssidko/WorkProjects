#ifndef _DMU_H
#define _DMU_H

#include "zfs_type.h"

typedef enum dmu_object_byteswap {
	DMU_BSWAP_UINT8,
	DMU_BSWAP_UINT16,
	DMU_BSWAP_UINT32,
	DMU_BSWAP_UINT64,
	DMU_BSWAP_ZAP,
	DMU_BSWAP_DNODE,
	DMU_BSWAP_OBJSET,
	DMU_BSWAP_ZNODE,
	DMU_BSWAP_OLDACL,
	DMU_BSWAP_ACL,
	/*
	* Allocating a new byteswap type number makes the on-disk format
	* incompatible with any other format that uses the same number.
	*
	* Data can usually be structured to work with one of the
	* DMU_BSWAP_UINT* or DMU_BSWAP_ZAP types.
	*/
	DMU_BSWAP_NUMFUNCS
} dmu_object_byteswap_t;

#define	DMU_OT_NEWTYPE 0x80
#define	DMU_OT_METADATA 0x40
#define	DMU_OT_BYTESWAP_MASK 0x3f

/*
* Defines a uint8_t object type. Object types specify if the data
* in the object is metadata (boolean) and how to byteswap the data
* (dmu_object_byteswap_t).
*/
#define	DMU_OT(byteswap, metadata) \
	(DMU_OT_NEWTYPE | \
	((metadata) ? DMU_OT_METADATA : 0) | \
	((byteswap) & DMU_OT_BYTESWAP_MASK))

#define	DMU_OT_IS_VALID(ot) (((ot) & DMU_OT_NEWTYPE) ? \
	((ot) & DMU_OT_BYTESWAP_MASK) < DMU_BSWAP_NUMFUNCS : \
	(ot) < DMU_OT_NUMTYPES)

#define	DMU_OT_IS_METADATA(ot) (((ot) & DMU_OT_NEWTYPE) ? \
	((ot) & DMU_OT_METADATA) : \
	dmu_ot[(int)(ot)].ot_metadata)

typedef enum dmu_object_type {
	DMU_OT_NONE,
	/* general: */
	DMU_OT_OBJECT_DIRECTORY,		/* ZAP */
	DMU_OT_OBJECT_ARRAY,			/* UINT64 */
	DMU_OT_PACKED_NVLIST,			/* UINT8 (XDR by nvlist_pack/unpack) */
	DMU_OT_PACKED_NVLIST_SIZE,		/* UINT64 */
	DMU_OT_BPOBJ,					/* UINT64 */
	DMU_OT_BPOBJ_HDR,				/* UINT64 */
	/* spa: */
	DMU_OT_SPACE_MAP_HEADER,		/* UINT64 */
	DMU_OT_SPACE_MAP,				/* UINT64 */
	/* zil: */
	DMU_OT_INTENT_LOG,				/* UINT64 */
	/* dmu: */
	DMU_OT_DNODE,					/* DNODE */
	DMU_OT_OBJSET,					/* OBJSET */
	/* dsl: */
	DMU_OT_DSL_DIR,					/* UINT64 */
	DMU_OT_DSL_DIR_CHILD_MAP,		/* ZAP */
	DMU_OT_DSL_DS_SNAP_MAP,			/* ZAP */
	DMU_OT_DSL_PROPS,				/* ZAP */
	DMU_OT_DSL_DATASET,				/* UINT64 */
	/* zpl: */
	DMU_OT_ZNODE,					/* ZNODE */
	DMU_OT_OLDACL,					/* Old ACL */
	DMU_OT_PLAIN_FILE_CONTENTS,		/* UINT8 */
	DMU_OT_DIRECTORY_CONTENTS,		/* ZAP */
	DMU_OT_MASTER_NODE,				/* ZAP */
	DMU_OT_UNLINKED_SET,			/* ZAP */
	/* zvol: */
	DMU_OT_ZVOL,					/* UINT8 */
	DMU_OT_ZVOL_PROP,				/* ZAP */
	/* other; for testing only! */
	DMU_OT_PLAIN_OTHER,				/* UINT8 */
	DMU_OT_UINT64_OTHER,			/* UINT64 */
	DMU_OT_ZAP_OTHER,				/* ZAP */
	/* new object types: */
	DMU_OT_ERROR_LOG,				/* ZAP */
	DMU_OT_SPA_HISTORY,				/* UINT8 */
	DMU_OT_SPA_HISTORY_OFFSETS,		/* spa_his_phys_t */
	DMU_OT_POOL_PROPS,				/* ZAP */
	DMU_OT_DSL_PERMS,				/* ZAP */
	DMU_OT_ACL,						/* ACL */
	DMU_OT_SYSACL,					/* SYSACL */
	DMU_OT_FUID,					/* FUID table (Packed NVLIST UINT8) */
	DMU_OT_FUID_SIZE,				/* FUID table size UINT64 */
	DMU_OT_NEXT_CLONES,				/* ZAP */
	DMU_OT_SCAN_QUEUE,				/* ZAP */
	DMU_OT_USERGROUP_USED,			/* ZAP */
	DMU_OT_USERGROUP_QUOTA,			/* ZAP */
	DMU_OT_USERREFS,				/* ZAP */
	DMU_OT_DDT_ZAP,					/* ZAP */
	DMU_OT_DDT_STATS,				/* ZAP */
	DMU_OT_SA,						/* System attr */
	DMU_OT_SA_MASTER_NODE,			/* ZAP */
	DMU_OT_SA_ATTR_REGISTRATION,	/* ZAP */
	DMU_OT_SA_ATTR_LAYOUTS,			/* ZAP */
	DMU_OT_SCAN_XLATE,				/* ZAP */
	DMU_OT_DEDUP,					/* fake dedup BP from ddt_bp_create() */
	DMU_OT_DEADLIST,				/* ZAP */
	DMU_OT_DEADLIST_HDR,			/* UINT64 */
	DMU_OT_DSL_CLONES,				/* ZAP */
	DMU_OT_BPOBJ_SUBOBJ,			/* UINT64 */
	/*
	 * Do not allocate new object types here. Doing so makes the on-disk
	 * format incompatible with any other format that uses the same object
	 * type number.
	 *
	 * When creating an object which does not have one of the above types
	 * use the DMU_OTN_* type with the correct byteswap and metadata
	 * values.
	 *
	 * The DMU_OTN_* types do not have entries in the dmu_ot table,
	 * use the DMU_OT_IS_METDATA() and DMU_OT_BYTESWAP() macros instead
	 * of indexing into dmu_ot directly (this works for both DMU_OT_* types
	 * and DMU_OTN_* types).
	 */
	DMU_OT_NUMTYPES,

	/*
	 * Names for valid types declared with DMU_OT().
	 */
	DMU_OTN_UINT8_DATA = DMU_OT(DMU_BSWAP_UINT8, B_FALSE),
	DMU_OTN_UINT8_METADATA = DMU_OT(DMU_BSWAP_UINT8, B_TRUE),
	DMU_OTN_UINT16_DATA = DMU_OT(DMU_BSWAP_UINT16, B_FALSE),
	DMU_OTN_UINT16_METADATA = DMU_OT(DMU_BSWAP_UINT16, B_TRUE),
	DMU_OTN_UINT32_DATA = DMU_OT(DMU_BSWAP_UINT32, B_FALSE),
	DMU_OTN_UINT32_METADATA = DMU_OT(DMU_BSWAP_UINT32, B_TRUE),
	DMU_OTN_UINT64_DATA = DMU_OT(DMU_BSWAP_UINT64, B_FALSE),
	DMU_OTN_UINT64_METADATA = DMU_OT(DMU_BSWAP_UINT64, B_TRUE),
	DMU_OTN_ZAP_DATA = DMU_OT(DMU_BSWAP_ZAP, B_FALSE),
	DMU_OTN_ZAP_METADATA = DMU_OT(DMU_BSWAP_ZAP, B_TRUE),
} dmu_object_type_t;

typedef enum dmu_objset_type {
	DMU_OST_NONE,
	DMU_OST_META,
	DMU_OST_ZFS,
	DMU_OST_ZVOL,
	DMU_OST_OTHER,			/* For testing only! */
	DMU_OST_ANY,			/* Be careful! */
	DMU_OST_NUMTYPES
} dmu_objset_type_t;

/*
* The names of zap entries in the DIRECTORY_OBJECT of the MOS.
*/
#define	DMU_POOL_DIRECTORY_OBJECT		1
#define	DMU_POOL_CONFIG					"config"
#define	DMU_POOL_FEATURES_FOR_WRITE		"features_for_write"
#define	DMU_POOL_FEATURES_FOR_READ		"features_for_read"
#define	DMU_POOL_FEATURE_DESCRIPTIONS	"feature_descriptions"
#define	DMU_POOL_FEATURE_ENABLED_TXG	"feature_enabled_txg"
#define	DMU_POOL_ROOT_DATASET			"root_dataset"
#define	DMU_POOL_SYNC_BPOBJ				"sync_bplist"
#define	DMU_POOL_ERRLOG_SCRUB			"errlog_scrub"
#define	DMU_POOL_ERRLOG_LAST			"errlog_last"
#define	DMU_POOL_SPARES					"spares"
#define	DMU_POOL_DEFLATE				"deflate"
#define	DMU_POOL_HISTORY				"history"
#define	DMU_POOL_PROPS					"pool_props"
#define	DMU_POOL_L2CACHE				"l2cache"
#define	DMU_POOL_TMP_USERREFS			"tmp_userrefs"
#define	DMU_POOL_DDT					"DDT-%s-%s-%s"
#define	DMU_POOL_DDT_STATS				"DDT-statistics"
#define	DMU_POOL_CREATION_VERSION		"creation_version"
#define	DMU_POOL_SCAN					"scan"
#define	DMU_POOL_FREE_BPOBJ				"free_bpobj"
#define	DMU_POOL_BPTREE_OBJ				"bptree_obj"
#define	DMU_POOL_EMPTY_BPOBJ			"empty_bpobj"

#endif // _DMU_H
