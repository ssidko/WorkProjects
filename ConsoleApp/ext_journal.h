#pragma once

#include "W32Lib.h"

namespace ext {

class Journal
{

	typedef struct HEADER {
		DWORD magic;
		DWORD type;
		DWORD seq_number;
	} HEADER;

	enum {
		kDescriptorBlock	 = 1,
		kAcceptedBlock,
		kSuperBlockV1,
		kSuperBlockV2,
		kCancelBlock
	};

	typedef struct SUPER_BLOCK{
		HEADER header;
		DWORD block_size;
		DWORD blocks_count;
		DWORD start_block;
		DWORD first_transaction_number;
		DWORD first_block_first_transaction;
		DWORD error;
		// EXT_JORNAL_HEADER::type == kSuperBlockV2
		DWORD supported_features;
		DWORD unsupported_features;
		DWORD ro_supported_features;
		uuid_t uuid;
		DWORD fs_count;
		DWORD sb_copy;
		DWORD max_jrn_blocks_for_transaction;
		DWORD max_sys_blocks_for_transaction;
		BYTE reserved[176];
		uuid_t fs[48];
	} SUPER_BLOCK;

	typedef struct DESCRIPTOR_BLOCK_ENTRY {
		DWORD fs_block;
		DWORD flags;
		uuid_t uuid;
	} DESCRIPTOR_BLOCK_ENTRY;

	enum {
		kSpecialProcessing	 = 0x01,
		kSameUUID			 = 0x02,
		kDeletedBlock		 = 0x04,
		kLastEntry			 = 0x08
	};

	typedef struct _CANCEL_BLOCK {
		HEADER header;
		DWORD size;
		DWORD blocks[1];
	} CANCEL_BLOCK;

private:
	SUPER_BLOCK sb;

public:
	Journal(void);
	~Journal(void);
};

} // namespace ext

