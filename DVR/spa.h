#ifndef _SPA_H
#define _SPA_H

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
#define	SPA_MINBLOCKSHIFT					9
#define	SPA_OLD_MAXBLOCKSHIFT				17
#define	SPA_MAXBLOCKSHIFT					24
#define	SPA_MINBLOCKSIZE					(1ULL << SPA_MINBLOCKSHIFT)
#define	SPA_OLD_MAXBLOCKSIZE				(1ULL << SPA_OLD_MAXBLOCKSHIFT)
#define	SPA_MAXBLOCKSIZE					(1ULL << SPA_MAXBLOCKSHIFT)

#pragma pack(push, 1)

typedef struct zio_cksum {
	uint64_t	word[4];
} zio_cksum_t;

/*
* Embedded checksum
*/
#define	ZEC_MAGIC	0x210da7ab10c7a11ULL		// zio-data-block-tail

typedef struct zio_eck {
	uint64_t	zec_magic;			/* for validation, endianness	*/
	zio_cksum_t	zec_cksum;			/* 256-bit checksum		*/
} zio_eck_t;

typedef struct dva {
	uint64_t	alloc_size : 24;			/* allocated size in sectors (including RAID-Z parity and gang block headers) */
	uint64_t	grid : 8;					/* RAID-Z layout information (reserved for future use) */
	uint64_t	vdev_id : 32;				/* virtual device ID */
	uint64_t	offset : 63;				/* offset in sectors (512 bytes) */
	uint64_t	gang_flag : 1;				/* gang block indicator */
} dva_t;

typedef struct blk_props {
	uint64_t	logical_size : 16;			/* logical size */
	uint64_t	physical_size : 16;			/* physical size (after compression) */
	uint64_t	compression : 7;			/* compression function */
	uint64_t	embedded : 1;				/* blkptr_t contains embedded data (see below) blk_props_emb_t */
	uint64_t	checksum : 8;				/* checksum function */
	uint64_t	type : 8;					/* DMU object type */
	uint64_t	level : 5;					/* level of indirection */
	uint64_t	encryption : 1;				/* encryption (on version 30, which is not supported) */
	uint64_t	dedup : 1;					/* deduplication */
	uint64_t	byteorder : 1;				/* byteorder (endianness)*/
} blk_props_t;

typedef struct blk_props_emb {
	uint64_t	logical_size : 25;			/* logical size */
	uint64_t	physical_size : 7;			/* physical size (after compression) */
	uint64_t	compression : 7;			/* compression function */
	uint64_t	embedded : 1;				/* set to one */
	uint64_t	etype : 8;					/* how to interpret embedded data (BP_EMBEDDED_TYPE_*) */
	uint64_t	type : 8;					/* DMU object type */
	uint64_t	level : 5;					/* level of indirection */
	uint64_t	encryption : 1;				/* encryption (on version 30, which is not supported) */
	uint64_t	dedup : 1;					/* deduplication */
	uint64_t	byteorder : 1;				/* byteorder (endianness)*/
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

typedef struct blkptr_emb {
	uint64_t		payload1[6];
	blk_props_emb_t	props;
	uint64_t		payload2[3];
	uint64_t		log_birth;
	uint64_t		payload3[5];
} blkptr_emb_t;

#define	BPE_NUM_WORDS		14
#define	BPE_PAYLOAD_SIZE	(BPE_NUM_WORDS * sizeof (uint64_t))

typedef enum bp_embedded_type {
	BP_EMBEDDED_TYPE_DATA,
	BP_EMBEDDED_TYPE_RESERVED, /* Reserved for an unintegrated feature. */
	NUM_BP_EMBEDDED_TYPES = BP_EMBEDDED_TYPE_RESERVED
} bp_embedded_type_t;

#pragma pack(pop)

#define	BP_IS_EMBEDDED(bp)		!!bp.props.embedded

#define	DVA_GET_ASIZE(dva_)		dva_.alloc_size

#define	BP_GET_NDVAS(bp)	\
	(BP_IS_EMBEDDED(bp) ? 0 : \
	!!DVA_GET_ASIZE((bp).dva[0]) + \
	!!DVA_GET_ASIZE((bp).dva[1]) + \
	!!DVA_GET_ASIZE((bp).dva[2]))

#endif // SPA
