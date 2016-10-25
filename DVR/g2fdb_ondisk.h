#ifndef _G2FDB_ONDISK_H
#define _G2FDB_ONDISK_H

#include "windows.h"
#include "Timestamp.h"

//
// DVR Infinity (example: NDR-X2416)
//
// This is G2FDb Magic
// This is G2FDb Storage Magic
//

namespace G2FDB
{
#pragma pack(push)
#pragma pack(1)

	struct TIMESTAMP {
		DWORD seconds : 6;
		DWORD minutes : 6;
		DWORD hours : 5;
		DWORD day : 5;
		DWORD month : 4;
		DWORD year : 6;

		void Timestamp(dvr::Timestamp &t) {
			t.year	= (year + 1970);
			t.month	= month;
			t.day	= day;
			t.hours = hours;
			t.minutes = minutes;
			t.seconds = seconds;
		}
	};

#define FRAME_HEADER_SIZE	((DWORD)0xA1)

	struct FrameHeader {
		DWORD signature;
		TIMESTAMP time;						// 0x00000002
		DWORD unk_1;
		DWORD unk_2;						// 0x03419F7D may be signature
		DWORD data_size;
		DWORD unk_3;
		WORD camera;
		WORD unk_4;
		DWORD frame_offset;
		WORD width;
		WORD height;
		WORD str_length;
		BYTE string[92];
		BYTE unk_5[33];
	};

#pragma pack(pop)





}

#endif // _G2FDB_ONDISK_H