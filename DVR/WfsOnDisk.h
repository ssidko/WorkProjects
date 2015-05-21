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

}

#endif // _WFS_ON_DISK_H