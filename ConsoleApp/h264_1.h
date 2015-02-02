#pragma once
#include <windows.h>


namespace h264_1
{
#pragma pack (push)
#pragma pack(1)

	typedef enum {
		kType_0A		= 0x0A,
		kType_0D		= 0x0D,
		kFrameHeader	= 0x0C,
	} Type;

	typedef struct _TIMESTAMP {
		DWORD seconds:6;
		DWORD minutes:6;
		DWORD hours:5;
		DWORD day:5;
		DWORD month:4;
		DWORD year:6;	
	} TIMESTAMP;

#define HDR_MARK_1		((DWORD)0x010000)
#define HDR_MARK_2		((DWORD)0x0F)

	typedef struct _HEADER {
		DWORD mark_1:24;	// 0x010000
		DWORD type:4;
		DWORD mark_2:4;		// 0x0F
		bool IsValid(void) { return ((this->mark_1 == HDR_MARK_1) && (this->mark_2 == HDR_MARK_2)); }
	} HEADER;

	typedef struct  _SUBFRAME_TIME{
		HEADER hdr;
		DWORD unknown_1;
		TIMESTAMP time;
		DWORD data_size;
		BYTE *data;
	} SUBFRAME_TIME;

	typedef struct  _SUBFRAME_0D{
		HEADER hdr;
		DWORD data_size;
		BYTE *data;
	} SUBFRAME_0D;

	typedef struct  _SUBFRAME_0A{
		HEADER hdr;
		BYTE unk_1;
		BYTE unk_2;
		WORD data_size;
		BYTE *data;
	} SUBFRAME_0A;

#pragma pack (pop)


	typedef struct _FRAME_DESCRIPTOR{
		LONGLONG offset;
		LONGLONG size;
	} FRAME_DESCRIPTOR;

	int main(TCHAR *file_name, LONGLONG offset, TCHAR *out_dir);

}

