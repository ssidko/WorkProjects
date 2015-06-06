#include <windows.h>

//
// mdadm super_block
// https://raid.wiki.kernel.org/index.php/RAID_superblock_formats
//

#define MDADM_SUPER_BLOCK_OFFSET			4096			//	bytes
#define MDADM_SUPER_BLOCK_MAGIC				0xA92B4EFC

struct _MDADM_SUPER_BLOCK {

	//
	// Superblock/"Magic-Number" Identification area
	//
	DWORD magic;			// MDADM_SUPER_BLOCK_MAGIC
	DWORD version;

	DWORD feature_map;		// 1 - RAID Bitmap is used
							// 2 - RAID Recovery is in progress
							// 4 - RAID Reshape is in progress

	BYTE pad_0[4];			// 0x00

	//
	// Per-Array Identification & Configuration area
	//
	BYTE array_uuid[16];
	BYTE array_name[32];
	LONGLONG creation_time;

	DWORD raid_level;		// RAID Level of the Array
							// '-4' - Multi-Path
							// '-1' - Linear
							// 0 - RAID-0 (Striped)
							// 1 - RAID-1 (Mirrored)
							// 4 - RAID-4
							// 5 - RAID-5
							// 6 - RAID-6
							// A - RAID-10

	DWORD raid_layout;		// 0 - left asymmetric
							// 1 - right asymmetric
							// 2 - left symmetric (default)
							// 3 - right asymetric

	DWORD unk_5;
	LONGLONG dev_size;		// Размер тома в секторах.
	DWORD chunk_size;		// Размер блока в секторах
	DWORD dev_in_array;		// Кол-во дисков в масиве ??
	DWORD unk_7;
	DWORD unk_8;
	LONGLONG array_size;	// Размер масива в секторах.
	DWORD unk_9;
	DWORD unk_10;
	LONGLONG unk_11;
	LONGLONG start_array;	// Начало масива, в секторах. Именно осюда RAID начинает тусовать блоки.
	LONGLONG end_array;		// Конец масива, в секторах.
	BYTE unk_12[16];
	LONGLONG dev_number;	// Номер диска в масиве.
	BYTE dev_guid[16];
	LONGLONG unk_13;
	DWORD last_mount_time;

}