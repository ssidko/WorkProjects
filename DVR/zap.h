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
#define	MZAP_MAX_BLKSZ				1 << 17								// SPA_OLD_MAXBLOCKSIZE

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
	uint64_t block_type;			/* ZBT_HEADER */
	uint64_t magic;					/* ZAP_MAGIC */

	struct zap_table_phys {
		uint64_t start_blk;			/* starting block number */
		uint64_t num_blks;			/* number of blocks */
		uint64_t shift;				/* bits to index it */
		uint64_t next_blk;			/* next (larger) copy start block */
		uint64_t blks_copied;		/* number source blocks copied */
	} ptrtbl;

	uint64_t free_blk;				/* the next free block */
	uint64_t num_leafs;				/* number of leafs */
	uint64_t num_entries;			/* number of entries */
	uint64_t salt;					/* salt to stir into hash function */
	uint64_t norm_flags;			/* flags for u8_textprep_str() */
	uint64_t flags;					/* zap_flags_t */
	/*
	* This structure is followed by padding, and then the embedded
	* pointer table.  The embedded pointer table takes up second
	* half of the block.  It is accessed using the
	* ZAP_EMBEDDED_PTRTBL_ENT() macro.
	*/
} zap_phys_t;

#define	ZAP_LEAF_MAGIC					0x2AB1EAF

/* chunk size = 24 bytes */
#define	ZAP_LEAF_CHUNKSIZE				24
#define ZAP_LEAF_HASH_ENTRY_SIZE		2

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
* block size (1<<block_size_shift) - hash entry size (2) * number of hash
* entries - header space (2*chunksize)
*/
#define	ZAP_LEAF_NUMCHUNKS(block_size_shift) \
	( ((1 << block_size_shift) - 2*ZAP_LEAF_HASH_NUMENTRIES(block_size_shift)) / ZAP_LEAF_CHUNKSIZE - 2 )

/*
* The amount of space within the chunk available for the array is:
* chunk size - space for type (1) - space for next pointer (2)
*/
#define	ZAP_LEAF_ARRAY_BYTES			(ZAP_LEAF_CHUNKSIZE - 3)

#define	ZAP_LEAF_ARRAY_NCHUNKS(bytes)	(((bytes)+ZAP_LEAF_ARRAY_BYTES-1)/ZAP_LEAF_ARRAY_BYTES)


typedef struct zap_leaf_phys {
	struct zap_leaf_header {
		/* Public to ZAP */
		uint64_t block_type;		/* ZBT_LEAF */
		uint64_t pad1;
		uint64_t prefix;			/* hash prefix of this leaf */
		uint32_t magic;				/* ZAP_LEAF_MAGIC */
		uint16_t nfree;				/* number free chunks */
		uint16_t nentries;			/* number of entries */
		uint16_t prefix_len;		/* num bits used to id this */

		/* Private to zap_leaf */
		uint16_t freelist;			/* chunk head of free list */
		uint8_t flags;				/* ZLF_* flags */
		uint8_t pad2[11];
	} hdr;							/* 2 24-byte chunks */

	/*
	* The header is followed by a hash table with
	* ZAP_LEAF_HASH_NUMENTRIES(zap) entries.  The hash table is
	* followed by an array of ZAP_LEAF_NUMCHUNKS(zap)
	* zap_leaf_chunk structures.  These structures are accessed
	* with the ZAP_LEAF_CHUNK() macro.
	*/

	uint16_t hash[1];
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
		uint8_t type; 				/* always ZAP_CHUNK_ENTRY */
		uint8_t value_intlen;		/* size of value's ints */
		uint16_t next;				/* next entry in hash chain */
		uint16_t name_chunk;		/* first chunk of the name */
		uint16_t name_numints;		/* ints in name (incl null) */
		uint16_t value_chunk;		/* first chunk of the value */
		uint16_t value_numints;		/* value length in ints */
		uint32_t cd;				/* collision differentiator */
		uint64_t hash;				/* hash value of the name */
	} entry;
	struct zap_leaf_array {
		uint8_t type;				/* always ZAP_CHUNK_ARRAY */
		uint8_t array[ZAP_LEAF_ARRAY_BYTES];
		uint16_t next;				/* next blk or CHAIN_END */
	} array;
	struct zap_leaf_free {
		uint8_t type;				/* always ZAP_CHUNK_FREE */
		uint8_t pad[ZAP_LEAF_ARRAY_BYTES];
		uint16_t next;				/* next in free list, or CHAIN_END */
	} free;
} zap_leaf_chunk_t;

#pragma pack(pop)

using NVPair = std::pair<uint64_t, std::string>;
using NVPairList = std::map<uint64_t, std::string>;

bool MZapObjectToNVPairs(const std::vector<char> &mzap_obj, NVPairList &pairs);
void TraversingMicroZapEntries(const std::vector<char> &mzap_obj, std::function<void(const uint64_t&, const char*)> callback);

#endif // _ZAP_H