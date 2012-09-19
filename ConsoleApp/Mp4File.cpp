#include "Mp4File.h"


BOOL Mp4File::CheckStructure()
{
	MP4_HEADER hdr;
	SetPointer(0);
	if (sizeof(hdr) == Read(&hdr, sizeof(hdr))) {
		
	}
	return TRUE;
}