#pragma once
#include <windows.h>
#include <W32Lib.h>


namespace h264_1
{
#pragma pack (push)
#pragma pack(1)

	typedef enum {
		kUnknownType	= 0x00,
		kType_0A		= 0x0A,
		kType_0D		= 0x0D,
		kWithTimestamp	= 0x0C,
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

	typedef struct _COMMON_HEADER {
		DWORD mark_1:24;	// 0x010000
		DWORD type:4;
		DWORD mark_2:4;		// 0x0F
		bool IsValid(void) { return ((this->mark_1 == HDR_MARK_1) && (this->mark_2 == HDR_MARK_2)); }
	} COMMON_HEADER;

	typedef struct  _SUBFRAME_TIME{
		COMMON_HEADER hdr;
		DWORD unknown_1;
		TIMESTAMP time;
		DWORD data_size;
	} SUBFRAME_TIME;

	typedef struct  _SUBFRAME_0A{
		COMMON_HEADER hdr;
		BYTE unk_1;
		BYTE unk_2;
		WORD data_size;
	} SUBFRAME_0A;

	typedef struct  _SUBFRAME_0D{
		COMMON_HEADER hdr;
		DWORD data_size;
	} SUBFRAME_0D;

	typedef struct  _SUBFRAME_HEADER{
		DWORD type;
		bool unknown_type;
		DWORD header_size;
		DWORD data_size;
		LONGLONG offset;
		TIMESTAMP timestamp;
	} SUBFRAME_HEADER;

#pragma pack (pop)


	typedef struct _FRAME_DESCRIPTOR{
		DWORD subframes_count;
		LONGLONG offset;
		LONGLONG size;
		LONGLONG clean_size;
		TIMESTAMP timestamp;
		bool IsFull(void) {return (size == clean_size);}
	} FRAME_DESCRIPTOR;


	bool ReadSubframeHeader(FileEx &file, LONGLONG &offset, SUBFRAME_HEADER &subframe_header);
	bool ReadSubframeHeader(FileEx &file, SUBFRAME_HEADER &subframe_header);

	int main(TCHAR *file_name, LONGLONG offset, TCHAR *out_dir);

}

