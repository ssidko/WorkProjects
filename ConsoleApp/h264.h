#pragma once

#ifndef _H264DVR_H
#define _H264DVR_H

#include "W32Lib.h"
#include <windows.h>
#include <assert.h>
#include <vector>
#include <ctime>
#include "BufferedFile.h"

namespace h264dvr
{
//#define MIN_TIMESTAMP			(DWORD) 0x52C35A80		// 01.01.2014
//#define MAX_TIMESTAMP			(DWORD)	0x5685C180		// 01.01.2016

#define MIN_TIMESTAMP			(DWORD) 0x54026580		// 31.08.2014
#define MAX_TIMESTAMP			(DWORD)	0x5405087F		// 01.09.2014


	extern DWORD min_time;
	extern DWORD max_time;

	enum {
		kMaxFrameSize = 0x0001FFFF,
	};

#pragma pack(push)
#pragma pack(1)

	typedef struct _MARKER{
		DWORD zeroes;
		BYTE one;

		bool IsValidMarker(void) { return ((zeroes == 0x00) && (one == 0x01)); }
		static bool IsValidMarker(const void*  const& marker) { return ((((_MARKER *)marker)->zeroes == 0x00) && (((_MARKER *)marker)->one == 0x01)); }
	} MARKER, *PMARKER;

	typedef struct _HEADER {
		BYTE channel_number;
		MARKER start_marker;
		BYTE unk2;
		BYTE unk3;
		BYTE unk4;
		BYTE unk5;
		BYTE unk6;
		BYTE unk7;
		DWORD counter;
		DWORD time_stamp;
		DWORD data_size;

		bool IsValidHeader(void) { return (data_size && start_marker.IsValidMarker() && IsValidDataSize() && IsValidTimestamp()); }
		bool IsValidDataSize(void) { return (data_size < h264dvr::kMaxFrameSize); }
		bool IsValidTimestamp(void) { return ((time_stamp >= min_time) && (time_stamp <= max_time)); }
	} HEADER, *PHEADER;

#pragma pack(pop)

	PHEADER ReadFrame(FileEx &file, std::vector<BYTE> &frame);
	PHEADER ReadFrame(BufferedFile &file, std::vector<BYTE> &frame);

	int SaveChannel(int ch_num);
	void SaveAllTimestampsToFile(void);
	int main(/*TCHAR *file_name, LONGLONG offset, TCHAR *out_dir*/void);

}

#endif // _H264DVR_H
