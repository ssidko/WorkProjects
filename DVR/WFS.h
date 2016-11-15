#ifndef _WFS_H
#define _WFS_H

#include <string>
#include "WfsScanner.h"
#include <sstream>

namespace WFS
{
	int Main(void);
	int StartRecovery(const std::string &volume_name, const std::string &out_directory, const Timestamp &start_time = dvr::Timestamp(), const Timestamp &end_time = dvr::Timestamp());
	std::string SequenceInfoString(FrameSequence &sequence);
}

#endif // _WFS_H

