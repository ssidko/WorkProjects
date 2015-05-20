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
		BufferedFile io;
	public:
		Scanner(const std::string &file_name);
		~Scanner(void);
		void NextFrame(const LONGLONG &offset);

	};


}

#endif // _WFS_SCANNER_H
