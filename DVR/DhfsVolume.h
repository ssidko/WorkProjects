#ifndef _DHFS_VOLUME_H
#define _DHFS_VOLUME_H

#include <windows.h>
#include <string>
#include <vector>

#include "Timestamp.h"
#include "BufferedFile.h"
#include "DhfsOnDisk.h"

namespace DHFS
{

	class Frame;
	class FrameSequence;

	class DhfsVolume
	{
	private:
		BufferedFile io;
	public:
		DhfsVolume(const std::string &volume_file);
		~DhfsVolume();
		bool Open(void);
		bool ReadFrame(Frame &frame);
		bool ReadFrameSequence(FrameSequence sequence);
	};

}

#endif // _DHFS_VOLUME_H

