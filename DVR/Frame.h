#ifndef _FRAME_H
#define _FRAME_H

#include <windows.h>
#include "Timestamp.h"
#include "File.h"
#include <string>

namespace DHFS
{
#define FRAME_HEADER_MAGIC			((DWORD)0x56414844)
#define FRAME_FOOTER_MAGIC			((DWORD)0x76616864)

#define FRAME_MAX_SIZE				((DWORD)2*1024*1024)

#pragma pack(push)
#pragma pack(1)

	typedef struct _TIMESTAMP {
		DWORD seconds:6;
		DWORD minutes:6;
		DWORD hours:5;
		DWORD day:5;
		DWORD month:4;
		DWORD year:6;	
	} TIMESTAMP;

	typedef struct _FRAME_HEADER {
		DWORD magic;					// FRAME_HEADER_MAGIC
		WORD flags;
		WORD camera;
		DWORD counter;
		DWORD size;
		TIMESTAMP time;
		BYTE unk1[8];
		DWORD unk2;
		BYTE data[1];
	} FRAME_HEADER;

	typedef struct _FRAME_FOOTER {
		DWORD magic;					// FRAME_FOOTER_MAGIC
		DWORD size;
	} FRAME_FOOTER;

#pragma pack(pop)

	typedef struct _FrameSequence {
		DWORD camera;
		LONGLONG offset;
		Timestamp timestamp;
		DWORD frame_count;
		DWORD start_counter;
		DWORD end_counter;
		DWORD size;
		void Clear(void) {memset(this, 0x00, sizeof(_FrameSequence));}
		void Info(std::string &info_str);
	} FrameSequence;

	class Frame
	{
	private:
		BYTE *buffer;
		LONGLONG offset;
		Timestamp timestamp;
		FRAME_HEADER *header;
		FRAME_FOOTER *footer;
		Frame(FRAME_HEADER &frame_header, LONGLONG &frame_offset);
		Frame(BYTE *frame_buffer, LONGLONG &frame_offset);
	public:
		~Frame(void);
		static bool IsValidHeader(FRAME_HEADER *hdr);
		static Frame *NextFrame(W32Lib::FileEx &file);
		bool IsNextFrame(Frame &next_frame);
		static bool NextFrameSequence(W32Lib::FileEx &file, FrameSequence &sequence);
		std::string Info(void);
		LONGLONG Offset(void) {return offset;}
		DWORD Size(void) {return header->size;}
		const Timestamp &Timestamp(void) {return timestamp;}
		const FRAME_HEADER &Header(void) {return *header;}
	};
}



#endif //_DHFS

