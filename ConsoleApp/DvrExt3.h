#pragma once
#ifndef	_DVR_EXT3_H
#define _DVR_EXT3_H

#include <windows.h>
#include <vector>
#include <set>

#include "BufferedFile.h"

namespace dvrext
{
#define EXT_BLOCK_SIZE				(DWORD)4096
#define EXT_VOLUME_OFFSET			(LONGLONG)10297665*512LL

#define EXT_MIN_BLOCK_NUMBER		4096
#define EXT_MAX_BLOCK_NUMBER		242902800

	template<DWORD BlockSize>
	union EXT_BLOCK {
		BYTE raw[BlockSize];
		DWORD dword[BlockSize / sizeof(DWORD)];
		inline void Erase(void) { memset(raw, 0x00, BlockSize); }
		inline DWORD Size(void) const { return BlockSize; }
		inline DWORD DwordsCount(void) const { return (BlockSize / sizeof(DWORD)); }
	};

	int SaveAllIndirectBlocksToFolder(std::string out_folder, std::string volume_file_name, const LONGLONG &offset, const DWORD &blocks_count, DWORD block_size);
	bool IsIndirectBlock(const EXT_BLOCK<EXT_BLOCK_SIZE> &block);
	int Run(void);
}

#endif // _DVR_EXT3_H
