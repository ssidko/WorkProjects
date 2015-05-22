#ifndef _WFS_SCANNER_H
#define _WFS_SCANNER_H

#include <windows.h>
#include <vector>

#include "Timestamp.h"

#include "WfsOnDisk.h"
#include "BufferedFile.h"

namespace WFS
{
	typedef struct _FrameInfo {
		DWORD type;
		LONGLONG offset;
		DWORD size;

		_FrameInfo() : type(0), offset(0), size(0) {}
		void Clear() {
			type = 0;
			offset = 0; 
			size = 0;
		}
	} FrameInfo;

	typedef struct _FrameSequence {
		Timestamp start_time;
		Timestamp end_time;
		DWORD frame_counter;
		DWORD size;
		LONGLONG offset;
		std::vector<BYTE> buffer;

		_FrameSequence() : frame_counter(0), size(0), offset(0) {}
		void Clear(void) {
			memset(&start_time, 0x00, sizeof(Timestamp));
			memset(&end_time, 0x00, sizeof(Timestamp));
			frame_counter = 0;
			size = 0;
			offset = 0;
			buffer.clear();
		}
	} FrameSequence;

	class Scanner
	{
	private:
		DWORD buffer_size;
		BYTE *buffer;
		BufferedFile io;
	public:
		BOOL ReadFrame(FrameSequence &seq, FrameInfo &readed_frame);
	public:
		Scanner(const std::string &file_name);
		~Scanner(void);
		BOOL Open(void);
		void Close(void);
		void SetPointer(const LONGLONG &offset);
		BOOL NextFrameHeader(FrameInfo &info);
		BOOL NextFrameWithTimestamp(void);
		BOOL NextFrameSequence(FrameSequence &sequence);
	};


}

#endif // _WFS_SCANNER_H
