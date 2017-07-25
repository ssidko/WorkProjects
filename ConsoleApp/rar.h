#ifndef _RAR_H
#define _RAR_H

#include <stdint.h>

namespace rar {

#pragma pack(push, 1)

	//
	//Archive file consists of variable length blocks.The order of these
	//blocks may vary, but the first block must be a marker block followed by
	//an archive header block.
	//

	//
	//Declared block types :
	//

	enum BlockType {
		MarkerBlock			= 0x72,
		ArchiveHeader		= 0x73,
		FileHeader			= 0x74,
		CommentHeader		= 0x75,
		ExtraInformation	= 0x76,
		OldSubblock			= 0x77,
		RecoveryRecord		= 0x78,
		AuthenticityInfo	= 0x79,
		NewSubblock			= 0x7A,
		Terminator			= 0x7B,
	};

	//
	//Each block begins with the following fields :
	//
	//HEAD_CRC       2 bytes     CRC of total block or block part
	//HEAD_TYPE      1 byte      Block type
	//HEAD_FLAGS     2 bytes     Block flags
	//HEAD_SIZE      2 bytes     Block size
	//ADD_SIZE       4 bytes     Optional field - added block size
	//

	struct BLOCK_HEADER {
		uint16_t crc;
		uint8_t type;
		uint16_t flags;
		uint16_t block_size;
		// uint16_t add_size;
	};

	//
	//Field ADD_SIZE present only if (HEAD_FLAGS & 0x8000) != 0
	//
	//Total block size is HEAD_SIZE if (HEAD_FLAGS & 0x8000) == 0
	//and HEAD_SIZE + ADD_SIZE if the field ADD_SIZE is present - when
	//(HEAD_FLAGS & 0x8000) != 0.
	//
	//In each block the followings bits in HEAD_FLAGS have the same meaning :
	//
	//0x4000 - if set, older RAR versions will ignore the block
	//and remove it when the archive is updated.
	//if clear, the block is copied to the new archive
	//file when the archive is updated;
	//
	//0x8000 - if set, ADD_SIZE field is present and the full block
	//size is HEAD_SIZE + ADD_SIZE.
	//
	//Comment block is actually used only within other blocks and doesn't
	//exist separately.

	// The marker block is actually considered as a fixed byte
	// Sequence : 0x52 0x61 0x72 0x21 0x1a 0x07 0x00

#define MARKER_BLOCK_CRC			0x6152
#define MARKER_BLOCK_FLAGS			0x1a21
#define MARKER_BLOCK_SIZE			0x0007

	struct FILE_HEADER {
		uint16_t crc;
		uint8_t type;
		uint16_t flags;
		uint16_t header_size;
		uint32_t compressed_size;
		uint32_t uncompressed_size;
		uint8_t os;
		uint32_t file_crc;
		uint32_t date_time;
		uint8_t min_unpack_ver;
		uint8_t packing_mathod;
		uint16_t name_size;
		uint32_t file_attributes;
		char file_name[1];
	};

	enum OperatingSystem {
		MsDOS	= 0,
		OS2		= 1,
		Windows = 2,
		Unix	= 3,
		MacOS	= 4,
		BeOS	= 5
	};

	enum PackingMethod {
		Storing				= 0x30,
		FastestCompression	= 0x31,
		FastCompression		= 0x32,
		NormalCompression	= 0x33,
		GoodCompression		= 0x34,
		BestCompression		= 0x35
	};

#pragma pack(pop)

}




#endif // _RAR_H