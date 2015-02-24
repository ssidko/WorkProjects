#include "dhfs.h"
#include <tchar.h>



bool DHFS::NextFrame(W32Lib::FileEx &file)
{
	LONGLONG offset = 0;
	DWORD header_magic = FRAME_HEADER_MAGIC;
	while ( offset = file.Find((BYTE *)&header_magic, sizeof(header_magic)) ) {
		FRAME_HEADER header = {0};
		if (file.Read((void *)&header, sizeof(header)) != sizeof(header)) {
			return false;
		}

		FRAME_FOOTER footer = {0};
		file.SetPointer(offset + header.size - sizeof(footer));
		if (file.Read((void *)&footer, sizeof(footer)) != sizeof(footer)) {
			return false;
		}
		if ((footer.magic == FRAME_FOOTER_MAGIC) && (header.size == footer.size)) {
			int x = 0;
		}
		
		file.SetPointer(offset + 1);
	}
	return false;
}

int DHFS::StartRecovering( void )
{
	W32Lib::FileEx file(_T("G:\\37025\\SAMSUNG HD103UJ 1AA01117.dsk"));
	if (!file.Open()) {
		return -1;
	}

	while (NextFrame(file)) {
		int x = 0;
	}

	return 0;
}
