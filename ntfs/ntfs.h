#ifndef _NTFS_H
#define _NTFS_H

#include <stdint.h>
#include <assert.h>

enum class AttributeType : uint32_t {
	UNUSED						= 0x00,
	STANDARD_INFORMATION		= 0x10,
	ATTRIBUTE_LIST				= 0x20,
	FILE_NAME					= 0x30,
	OBJECT_ID					= 0x40,
	SECURITY_DESCRIPTOR			= 0x50,
	VOLUME_NAME					= 0x60,
	VOLUME_INFORMATION			= 0x70,
	DATA						= 0x80,
	INDEX_ROOT					= 0x90,
	INDEX_ALLOCATION			= 0xA0,
	BITMAP						= 0xB0,
	REPARSE_POINT				= 0xC0,
	EA_INFORMATION				= 0xD0,
	EA							= 0xE0,
	PROPERTY_SET				= 0xF0,
	LOGGED_UTILITY_STREAM		= 0x100,
	FIRST_USER_DEFINED_ATTRIBUTE = 0x1000,
	END							= 0xFFFFFFFF,
};

#pragma pack(push, 1)

typedef struct {
	uint64_t number		: 48;
	uint64_t generation	: 16;
} record_base_adress_t;


typedef struct {
	uint32_t magic;				// 'FILE' 
	uint16_t usa_offset;		// Offset to the Update Sequence Array (usa)
	uint16_t usa_count;
	uint64_t lsn;				// $LogFile sequence number for this record
	uint16_t record_generation;	// Number of times this mft record has been reused
	uint16_t link_count;
	uint16_t attrs_offset;
	uint16_t flags;
	uint32_t used_size;
	uint32_t allocated_size;
	record_base_adress_t base_mft_record;
	uint16_t next_attr_id;
	/* sizeof() = 42 bytes */
} MFT_RECORD_OLD;

typedef struct {
	uint32_t magic;				// 'FILE' 
	uint16_t usa_offset;		// Offset to the Update Sequence Array (usa)
	uint16_t usa_count;
	uint64_t lsn;				// $LogFile sequence number for this record
	uint16_t record_generation;	// Number of times this mft record has been reused
	uint16_t link_count;
	uint16_t attrs_offset;
	uint16_t flags;
	uint32_t used_size;
	uint32_t allocated_size;
	record_base_adress_t base_mft_record;
	uint16_t next_attr_id;
	
	// The below fields are specific to NTFS 3.1+ (Windows XP and above):
	uint16_t reserved;			// Reserved/alignment.
	uint32_t mft_record_number;	// Number of this mft record.
} MFT_RECORD_NEW;

typedef struct {
	AttributeType attr_type;
	uint16_t entry_len;
	uint8_t name_len;
	uint8_t name_offset;
	uint64_t start_vcn;
	record_base_adress_t mft_record;
	uint8_t attr_id;
	uint8_t pad[7];
} ATTR_LIST_ENTRY;

static_assert(sizeof(ATTR_LIST_ENTRY) == 0x20, "Wrong attribute list entry size");

typedef struct {
	AttributeType attr_type;
	uint32_t attr_length;
	uint8_t non_resident;
	uint8_t name_length;
	uint16_t name_offset;
	uint8_t flags;
	uint16_t attr_id;

	union {
		// Resident attribute
		struct {
			uint32_t payload_size;
			uint16_t payload_offset;
		} res;

		// Non-resident attribute
		struct {
			uint64_t start_vcn;
			uint64_t end_vcn;
			uint16_t runlist_offset;
			uint16_t compression_block_size;
			uint8_t reserved1[4];		// Align to 8-byte boundary
			uint64_t allocated_size;
			uint64_t used_size;
			uint64_t initialized_size;

		} non_res;
	};

} ATTRIBUTE_RECORD;

#pragma pack(pop)

#endif // _NTFS_H