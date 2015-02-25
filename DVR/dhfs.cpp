#include "dhfs.h"
#include <tchar.h>

#include "Frame.h"

using namespace DHFS;

#include <string>

int DHFS::StartRecovering( void )
{
	W32Lib::FileEx file(_T("G:\\37025\\SAMSUNG HD103UJ 1AA01117.dsk"));
	W32Lib::FileEx log(_T("G:\\37025\\frame_info.txt"));
	if (!file.Open()) {
		return -1;
	}
	if (!log.Create()) {
		return -1;
	}

	Frame *frame = NULL;
	FrameSequence sequence;
	//LONGLONG offset = 251000312LL;
	//file.SetPointer(offset);

	while (Frame::NextFrameSequence(file, sequence)) {
		int x = 0;


		//std::string info = frame->Info();
		//log.Write(const_cast<void *>((void *)info.data()), info.size());
		//log.Write(const_cast<void *>((void *)separator.data()), separator.size());
		//delete frame;
	}

	return 0;
}
