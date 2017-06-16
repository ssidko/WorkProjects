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

/*
* the embedded pointer table takes up half a block:
* block size / entry size (2^3) / 2
*/
#define	ZAP_EMBEDDED_PTRTBL_SHIFT(block_size_shift) (block_size_shift - 3 - 1)

/*
* The embedded pointer table starts half-way through the block.  Since
* the pointer table itself is half the block, it starts at (64-bit)
* word number (1<<ZAP_EMBEDDED_PTRTBL_SHIFT(zap)).
*/
#define	ZAP_EMBEDDED_PTRTBL_ENT(zap_block, block_size_shift, idx) \
	((uint64_t *)zap_block) [(idx) + (1 << ZAP_EMBEDDED_PTRTBL_SHIFT(block_size_shift))]

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

#define	ZAP_LEAF_MAGIC  0x2AB1EAF

/* chunk size = 24 bytes */
#define	ZAP_LEAF_CHUNKSIZE 24

/*
* The leaf hash table has block size / 2^5 (32) number of entries,
* which should be more than enough for the maximum number of entries,
* which is less than block size / CHUNKSIZE (24) / minimum number of
* chunks per entry (3).
*/
#define	ZAP_LEAF_HASH_SHIFT(block_size_shift)		(block_size_shift - 5)
#define	ZAP_LEAF_HASH_NUMENTRIES(block_size_shift)	(1 << ZAP_LEAF_HASH_SHIFT(block_size_shift))

/*
* The amount of space available for chunks is:
* block size (1<<l->l_bs) - hash entry size (2) * number of hash
* entries - header space (2*chunksize)
*/
#define	ZAP_LEAF_NUMCHUNKS(block_size_shift) \
	(((1 << block_size_shift) - 2*ZAP_LEAF_HASH_NUMENTRIES(l)) / \
	ZAP_LEAF_CHUNKSIZE - 2)

/*
* The amount of space within the chunk available for the array is:
* chunk size - space for type (1) - space for next pointer (2)
*/
#define	ZAP_LEAF_ARRAY_BYTES			(ZAP_LEAF_CHUNKSIZE - 3)

#define	ZAP_LEAF_ARRAY_NCHUNKS(bytes)	(((bytes)+ZAP_LEAF_ARRAY_BYTES-1)/ZAP_LEAF_ARRAY_BYTES)

typedef struct zap_leaf_phys {
	struct zap_leaf_header {
		/* Public to ZAP */
		uint64_t lh_block_type;		/* ZBT_LEAF */
		uint64_t lh_pad1;
		uint64_t lh_prefix;			/* hash prefix of this leaf */
		uint32_t lh_magic;			/* ZAP_LEAF_MAGIC */
		uint16_t lh_nfree;			/* number free chunks */
		uint16_t lh_nentries;		/* number of entries */
		uint16_t lh_prefix_len;		/* num bits used to id this */

		/* Private to zap_leaf */
		uint16_t lh_freelist;		/* chunk head of free list */
		uint8_t lh_flags;			/* ZLF_* flags */
		uint8_t lh_pad2[11];
	} l_hdr;						/* 2 24-byte chunks */

	/*
	* The header is followed by a hash table with
	* ZAP_LEAF_HASH_NUMENTRIES(zap) entries.  The hash table is
	* followed by an array of ZAP_LEAF_NUMCHUNKS(zap)
	* zap_leaf_chunk structures.  These structures are accessed
	* with the ZAP_LEAF_CHUNK() macro.
	*/

	uint16_t l_hash[1];
} zap_leaf_phys_t;

typedef enum zap_chunk_type {
	ZAP_CHUNK_FREE		= 253,
	ZAP_CHUNK_ENTRY		= 252,
	ZAP_CHUNK_ARRAY		= 251,
	ZAP_CHUNK_TYPE_MAX	= 250
} zap_chunk_type_t;

#define	CHAIN_END 0xffff			/* end of the chunk chain */

typedef union zap_leaf_chunk {
	struct zap_leaf_entry {
		uint8_t le_type; 			/* always ZAP_CHUNK_ENTRY */
		uint8_t le_value_intlen;	/* size of value's ints */
		uint16_t le_next;			/* next entry in hash chain */
		uint16_t le_name_chunk;		/* first chunk of the name */
		uint16_t le_name_numints;	/* ints in name (incl null) */
		uint16_t le_value_chunk;	/* first chunk of the value */
		uint16_t le_value_numints;	/* value length in ints */
		uint32_t le_cd;				/* collision differentiator */
		uint64_t le_hash;			/* hash value of the name */
	} l_entry;
	struct zap_leaf_array {
		uint8_t la_type;			/* always ZAP_CHUNK_ARRAY */
		uint8_t la_array[ZAP_LEAF_ARRAY_BYTES];
		uint16_t la_next;			/* next blk or CHAIN_END */
	} l_array;
	struct zap_leaf_free {
		uint8_t lf_type;			/* always ZAP_CHUNK_FREE */
		uint8_t lf_pad[ZAP_LEAF_ARRAY_BYTES];
		uint16_t lf_next;			/* next in free list, or CHAIN_END */
	} l_free;
} zap_leaf_chunk_t;

#pragma pack(pop)


using NVPair = std::pair<uint64_t, std::string>;
using NVPairList = std::map<uint64_t, std::string>;

bool MZapObjectToNVPairs(const std::vector<char> &mzap_obj, NVPairList &pairs);
void TraversingMicroZapEntries(const std::vector<char> &mzap_obj, std::function<void(const uint64_t&, const char*)> callback);

void TraversingFatZapEntries(const std::vector<char> &zap_obj, std::function<void(const uint64_t&, const char*)> callback);

#endif // _ZAP_H