#ifndef _WFS_SCANNER_H
#define _WFS_SCANNER_H

#include <windows.h>

#include "WfsOnDisk.h"
#include "BufferedFile.h"

namespace WFS
{
	typedef struct _FrameDescriptor {
		DWORD type;
		LONGLONG offset;
		DWORD size;
	} FrameDescriptor;

	class Scanner
	{
	private:
		DWORD buffer_size;
		BYTE *buffer;
		BufferedFile io;
	public:
		Scanner(const std::string &file_name);
		~Scanner(void);
		BOOL Open(void);
		void Close(void);
		void SetPointer(const LONGLONG &offset);
		BOOL NextFrameHeader(LONGLONG &offset);
		BOOL NextFrame(LONGLONG &offset);

	};


}

#endif // _WFS_SCANNER_H
