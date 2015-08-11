#ifndef _WFS_ON_DISK_H
#define _WFS_ON_DISK_H

#include <windows.h>

namespace WFS
{
	typedef struct _TIMESTAMP {
		DWORD seconds : 6;
		DWORD minutes : 6;
		DWORD hours : 5;
		DWORD day : 5;
		DWORD month : 4;
		DWORD year : 6;
		operator Timestamp ()
		{
			return Timestamp(year, month, day, hours, minutes, seconds);
		}
	} TIMESTAMP;

	enum FrameType {
		kUnknownType = 0x00,
		kType_0A = 0x0A,
		kType_0D = 0x0D,
		kType_0C = 0x0C,
	};

#define WFS_FRAME_HEADER_MASK			((DWORD)0xF0FFFFFF)	
#define WFS_FRAME_HEADER_SIGNATURE		((DWORD)0xF0010000)

#define WFS_HDR_MARK_1					((DWORD)0x010000)
#define WFS_HDR_MARK_2					((DWORD)0x0F)

	typedef struct _FRAME_HEADER {
		DWORD mark_1 : 24;		// 0x010000	WFS_HDR_MARK_1
		DWORD type : 4;
		DWORD mark_2 : 4;		// 0x0F		WFS_HDR_MARK_2 
		bool IsValidSignature(void) { return ((this->mark_1 == WFS_HDR_MARK_1) && (this->mark_2 == WFS_HDR_MARK_2)); }
		bool IsValidType(void) { return ((type == kType_0A) || (type == kType_0D) || (type == kType_0C)); }
		bool IsValid(void) { return (IsValidSignature() && IsValidType()); }
	} FRAME_HEADER;

	typedef struct  _FRAME_0A {
		FRAME_HEADER hdr;
		BYTE unk_1;
		BYTE unk_2;
		WORD data_size;
	} FRAME_0A;

	typedef struct  _FRAME_0D {
		FRAME_HEADER hdr;
		DWORD data_size;
	} FRAME_0D;

	typedef struct  _FRAME_0C {
		FRAME_HEADER hdr;
		DWORD unk_1;
		TIMESTAMP time;
		DWORD data_size;
	} FRAME_0C;

#define	MAX_FRAME_HEADER_SIZE			((DWORD)sizeof(FRAME_0C))
#define	MAX_FRAME_SIZE					((DWORD)2*1024*1024)

	typedef struct _VOLUME_DESCRIPTOR {
		BYTE unk_1[16];					// Обычно - 0x00
		TIMESTAMP first_record_time;
		TIMESTAMP last_record_time;
		DWORD last_used_block;
		BYTE unk_3[16];					// Обычно - 0x00
		DWORD sector_size;
		DWORD block_size;				// Размер блока в секторах
		DWORD unk_4;					// 0x00
		DWORD reserved_blocks;			// Кол-во зарезервированных блоков - обычно 64
		DWORD cam_table_sector;			//
		DWORD descriptor_sector;
		DWORD index_table_sector;
		DWORD first_block_sector;		// Смещение к первому блоку в секторах
		DWORD total_blocks_count;		//
	} VOLUME_DESCRIPTOR;

}

#endif // _WFS_ON_DISK_H