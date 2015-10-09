#ifndef _EXTENT_RECOVERY_H
#define _EXTENT_RECOVERY_H

#include <windows.h>
#include <string>
#include "BufferedFile.h"

namespace ext4
{

#pragma pack(push)
#pragma pack(1)

	typedef struct _EXTENT {
		DWORD block;
		WORD length;
		WORD start_hi;
		DWORD start_lo;
		LONGLONG PysicalBlock(void) { return ((LONGLONG)start_lo) | (((LONGLONG)start_hi) << 32); }
	} EXTENT;

	typedef struct _EXTENT_IDX {
		DWORD block;
		DWORD leaf_lo;
		WORD leaf_hi;
		WORD unused;
		LONGLONG PysicalBlock(void) { return ((LONGLONG)leaf_lo) | (((LONGLONG)leaf_hi) << 32); }
	} EXTENT_IDX;

#define EXTENT_HEADER_MAGIC			(WORD)0xF30A

	typedef struct _EXTENT_HEADER {
		WORD magic;
		WORD entries;
		WORD max;
		WORD depth;
		DWORD generation;
	} EXTENT_HEADER;

	typedef struct _EXTENT_BLOCK {
		EXTENT_HEADER header;
		union {
			EXTENT extent[1];
			EXTENT_IDX extent_index[1];
		};
	} EXTENT_BLOCK;

#pragma pack(pop)

	class ExtentSaver
	{
	private:
		BufferedFile io;
		LONGLONG volume_offset;
		DWORD block_size;
		std::string out_dir;
		std::string volume_name;
		LONGLONG current_block;
		WORD max_extents_in_block;
	public:
		ExtentSaver(const std::string &result_out_dir, const std::string &ext_volume_file, const LONGLONG &ext_volume_offset, DWORD ext_block_size);
		~ExtentSaver(void);
		bool Open(void);
		bool SetPointer(const LONGLONG &offset);
		bool NextExtentBlock(LONGLONG &block_num);
		bool SaveToFile(const LONGLONG &block_num, W32Lib::FileEx *out_file);
		int Run();
	};
}

#endif //_EXTENT_RECOVERY_H