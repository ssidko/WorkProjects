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
		void Timestamp(Timestamp &t) {
			t.SetYear(2000 + year);
			t.SetMonth(month);
			t.SetDay(day);
			t.SetHours(hours);
			t.SetMinutes(minutes);
			t.SetSeconds(seconds);
		}
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
	} FRAME_HEADER;

	typedef struct _FRAME_FOOTER {
		DWORD magic;					// FRAME_FOOTER_MAGIC
		DWORD size;
	} FRAME_FOOTER;

#pragma pack(pop)

	typedef struct _FrameInfo {
		LONGLONG offset;
		Timestamp timestamp;
		DWORD camera;
		DWORD counter;
		DWORD size;
		void Clear(void) {memset(this, 0x00, sizeof(_FrameInfo));}
		void ToString(std::string &info_str);
	} FrameInfo;

	typedef struct _FrameSequenceInfo {
		FrameInfo start_frame;
		FrameInfo end_frame;
		DWORD frame_counter;
		DWORD size;
		void Clear(void) {memset(this, 0x00, sizeof(_FrameSequenceInfo));}
		bool IsYourNextFrame(FrameInfo &next_frame);
	} FrameSequenceInfo;

	class Volume
	{
	private:
		BufferedFile io;
	public:
		Volume(std::string &volume_file_name);
		bool Open();
		void SetPointer(LONGLONG &pointer);
		bool IsValidHeader(FRAME_HEADER &header);
		bool ReadFrame(std::vector<BYTE> &buffer, FrameInfo &info);
		bool NextFrame(std::vector<BYTE> &buffer, FrameInfo &info);
		bool NextFrameSequence(std::vector<BYTE> &sequence_buffer, FrameSequenceInfo &sequnence_info);
		void Test(void);
		void SaveFrameInfo(void);
	};
}



#endif //_DHFS

