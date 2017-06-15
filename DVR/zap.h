#ifndef _ZAP_H
#define _ZAP_H

#include <vector>
#include <map>
#include <string>
#include <functional>

#include "zfs_type.h"

#define	ZAP_MAXNAMELEN				256
#define	ZAP_MAXVALUELEN				(1024 * 8)
#define	ZAP_OLDMAXVALUELEN			1024
#define	ZFS_MAX_DATASET_NAME_LEN	256

#define	ZBT_LEAF					((1ULL << 63) + 0)
#define	ZBT_HEADER					((1ULL << 63) + 1)
#define	ZBT_MICRO					((1ULL << 63) + 3)
/* any other values are ptrtbl blocks */

#define	MZAP_ENT_LEN				64
#define	MZAP_NAME_LEN				(MZAP_ENT_LEN - 8 - 4 - 2)
#define MZAP_HEADER_SIZE			(8 * sizeof(uint64_t))				// 128 byte 

#pragma pack(push, 1)

typedef struct mzap_ent_phys {
	uint64_t mze_value;
	uint32_t mze_cd;
	uint16_t mze_pad;	/* in case we want to chain them someday */
	char mze_name[MZAP_NAME_LEN];
} mzap_ent_phys_t;

typedef struct mzap_phys {
	uint64_t mz_block_type;	/* ZBT_MICRO */
	uint64_t mz_salt;
	uint64_t mz_normflags;
	uint64_t mz_pad[5];
	mzap_ent_phys_t mz_chunk[1];
	/* actually variable size depending on block size */
} mzap_phys_t;

//
// Fat ZAP
// 

#define	ZAP_MAGIC		0x2F52AB2ABULL

typedef struct zap_phys {
	uint64_t zap_block_type;		/* ZBT_HEADER */
	uint64_t zap_magic;				/* ZAP_MAGIC */

	struct zap_table_phys {
		uint64_t zt_blk;			/* starting block number */
		uint64_t zt_numblks;		/* number of blocks */
		uint64_t zt_shift;			/* bits to index it */
		uint64_t zt_nextblk;		/* next (larger) copy start block */
		uint64_t zt_blks_copied;	/* number source blocks copied */
	} zap_ptrtbl;

	uint64_t zap_freeblk;			/* the next free block */
	uint64_t zap_num_leafs;			/* number of leafs */
	uint64_t zap_num_entries;		/* number of entries */
	uint64_t zap_salt;				/* salt to stir into hash function */
	uint64_t zap_normflags;			/* flags for u8_textprep_str() */
	uint64_t zap_flags;				/* zap_flags_t */
	/*
	* This structure is followed by padding, and then the embedded
	* pointer table.  The embedded pointer table takes up second
	* half of the block.  It is accessed using the
	* ZAP_EMBEDDED_PTRTBL_ENT() macro.
	*/
} zap_phys_t;

#pragma pack(pop)


using NVPair = std::pair<uint64_t, std::string>;
using NVPairList = std::map<uint64_t, std::string>;

bool MZapObjectToNVPairs(const std::vector<char> &mzap_obj, NVPairList &pairs);
void TraversingMicroZapEntries(const std::vector<char> &mzap_obj, std::function<void(const uint64_t&, const char*)> callback);

void TraversingFatZapEntries(const std::vector<char> &zap_obj, std::function<void(const uint64_t&, const char*)> callback);

#endif // _ZAP_H