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

	struct Frame {
		LONGLONG offset;
		std::vector<BYTE> data;
		void Clear(void);
	};


	class FrameSequence;

	class DhfsVolume
	{
	private:
		BufferedFile io;
		VOLUME_HEADER volume_header{ 0 };
	public:
		DhfsVolume(const std::string &volume_file);
		~DhfsVolume();
		bool Open(void);
		bool SetPointer(LONGLONG offset);
		LONGLONG Poiner(void);
		bool FindNextFrame(LONGLONG &offset);
		bool ReadFrame(Frame &frame);
		bool ReadFrameSequence(FrameSequence sequence);
	};

}

#endif // _DHFS_VOLUME_H

