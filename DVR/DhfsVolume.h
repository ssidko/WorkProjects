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
		FRAME_HEADER * Header(void);
		size_t PayloadOffset(void);
	};

	struct FrameSequence {
		LONGLONG offset;
		std::vector<BYTE> data;

		void AddFrame(Frame &frame);
		void AddFirstFrame(Frame &frame);
	};

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
		bool ReadFrame(Frame &frame);
		bool FindAndReadFrame(Frame &frame);
		bool FindAndReadFrameSequence(FrameSequence &sequence);
	};

}

#endif // _DHFS_VOLUME_H