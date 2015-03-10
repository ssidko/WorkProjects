#ifndef _FRAME_H
#define _FRAME_H

#include <windows.h>
#include "Timestamp.h"
#include "BufferedFile.h"
#include <string>
#include <vector>

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
		Timestamp start_time;
		Timestamp end_time;
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
		static Frame *NextFrame(BufferedFile &file);
		bool IsNextFrame(Frame &next_frame);
		static bool NextFrameSequence(BufferedFile &file, FrameSequence &sequence);
		std::string Info(void);
		LONGLONG Offset(void) {return offset;}
		DWORD Size(void) {return header->size;}
		const Timestamp &Time(void) {return timestamp;}
		const FRAME_HEADER &Header(void) {return *header;}
	};

	class FrameEx
	{
	private:
		std::vector<BYTE> buff;
		LONGLONG offset;
		Timestamp timestamp;
	public:
		FrameEx();
		~FrameEx() {}
		void Clear(void);
	};

	class FrameSequenceEx
	{
	private:
		std::vector<BYTE> buff;
	public:
		FrameSequenceEx();
		~FrameSequenceEx() {}
	};

	class Volume
	{
	private:
		BufferedFile io;
	public:
		Volume(std::string &volume_file_name);
		bool Open();
		bool NextFrame(FrameEx &frame);
	};
}



#endif //_DHFS

