#ifndef _DSL_H
#define _DSL_H

#include "zfs_type.h"

/*
* DD_FIELD_* are strings that are used in the "extensified" dsl_dir zap object.
* They should be of the format <reverse-dns>:<field>.
*/

#define	DD_FIELD_FILESYSTEM_COUNT	"com.joyent:filesystem_count"
#define	DD_FIELD_SNAPSHOT_COUNT		"com.joyent:snapshot_count"

typedef enum dd_used {
	DD_USED_HEAD,
	DD_USED_SNAP,
	DD_USED_CHILD,
	DD_USED_CHILD_RSRV,
	DD_USED_REFRSRV,
	DD_USED_NUM
} dd_used_t;

#define	DD_FLAG_USED_BREAKDOWN (1<<0)

#pragma pack(push, 1)

typedef struct dsl_dir_phys {
	uint64_t dd_creation_time;				/* not actually used */
	uint64_t dd_head_dataset_obj;
	uint64_t dd_parent_obj;
	uint64_t dd_origin_obj;
	uint64_t dd_child_dir_zapobj;
	/*
	* how much space our children are accounting for; for leaf
	* datasets, == physical space used by fs + snaps
	*/
	uint64_t dd_used_bytes;
	uint64_t dd_compressed_bytes;
	uint64_t dd_uncompressed_bytes;
	/* Administrative quota setting */
	uint64_t dd_quota;
	/* Administrative reservation setting */
	uint64_t dd_reserved;
	uint64_t dd_props_zapobj;
	uint64_t dd_deleg_zapobj;				/* dataset delegation permissions */
	uint64_t dd_flags;
	uint64_t dd_used_breakdown[DD_USED_NUM];
	uint64_t dd_clones;						/* dsl_dir objects */
	uint64_t dd_pad[13];					/* pad out to 256 bytes for good measure */
} dsl_dir_phys_t;

typedef struct dsl_dataset_phys {
	uint64_t ds_dir_obj;					/* DMU_OT_DSL_DIR */
	uint64_t ds_prev_snap_obj;				/* DMU_OT_DSL_DATASET */
	uint64_t ds_prev_snap_txg;
	uint64_t ds_next_snap_obj;				/* DMU_OT_DSL_DATASET */
	uint64_t ds_snapnames_zapobj;			/* DMU_OT_DSL_DS_SNAP_MAP 0 for snaps */
	uint64_t ds_num_children;				/* clone/snap children; ==0 for head */
	uint64_t ds_creation_time;				/* seconds since 1970 */
	uint64_t ds_creation_txg;
	uint64_t ds_deadlist_obj;				/* DMU_OT_DEADLIST */
	/*
	* ds_referenced_bytes, ds_compressed_bytes, and ds_uncompressed_bytes
	* include all blocks referenced by this dataset, including those
	* shared with any other datasets.
	*/
	uint64_t ds_referenced_bytes;
	uint64_t ds_compressed_bytes;
	uint64_t ds_uncompressed_bytes;
	uint64_t ds_unique_bytes;				/* only relevant to snapshots */
	/*
	* The ds_fsid_guid is a 56-bit ID that can change to avoid
	* collisions.  The ds_guid is a 64-bit ID that will never
	* change, so there is a small probability that it will collide.
	*/
	uint64_t ds_fsid_guid;
	uint64_t ds_guid;
	uint64_t ds_flags;						/* DS_FLAG_* */
	blkptr_t ds_bp;
	uint64_t ds_next_clones_obj;			/* DMU_OT_DSL_CLONES */
	uint64_t ds_props_obj;					/* DMU_OT_DSL_PROPS for snaps */
	uint64_t ds_userrefs_obj;				/* DMU_OT_USERREFS */
	uint64_t ds_pad[5];						/* pad out to 320 bytes for good measure */
} dsl_dataset_phys_t;

#pragma pack(pop)

#endif // _DSL_H
