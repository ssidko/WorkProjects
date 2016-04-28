#ifndef _WFS_H
#define _WFS_H

#include <string>
#include "WfsScanner.h"
#include <sstream>

namespace WFS
{
	int Main(void);
	int StartRecovering(const std::string &volume_name, const std::string &out_directory, const Timestamp &start_time, const Timestamp &end_time);
	std::string SequenceInfoString(FrameSequence &sequence);
	bool CreateNextVideoFile(void);
}

#endif // _WFS_H

