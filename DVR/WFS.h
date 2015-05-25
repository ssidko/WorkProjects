#ifndef _WFS_H
#define _WFS_H

#include <string>
#include "WfsScanner.h"
#include <sstream>

namespace WFS
{
	int Main(void);
	std::string SequenceInfoString(FrameSequence &sequence);
	bool CreateNextVideoFile(void);
}

#endif // _WFS_H

