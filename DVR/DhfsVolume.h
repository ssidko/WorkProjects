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
	struct FrameInfo {
		LONGLONG offset;
		BYTE camera;
		dvr::Timestamp time;

		void Clear(void);
	};

	struct Frame {
		FrameInfo info;
		std::vector<BYTE> data;

		void Clear(void);
		FRAME_HEADER * Header(void);
		size_t Size(void);
		size_t HeaderSize(void);
		size_t PayloadSize(void);
	};

	struct FrameSequence {
		FrameInfo first_frame;
		FrameInfo last_frame;
		size_t width;
		size_t height;
		size_t frames_count;
		std::vector<BYTE> data;

		void Clear(void);
		void AddFrame(Frame &frame);
		void AddFirstFrame(Frame &frame);
	};

	class DhfsVolume
	{
	private:
		BufferedFile io;
		VOLUME_HEADER volume_header{ 0 };
	public:
		DhfsVolume(const std::string &volume_file, const LONGLONG &size);
		~DhfsVolume();
		bool Open(void);
		bool SetPointer(LONGLONG offset);
		LONGLONG Poiner(void);
		bool ReadFrame(Frame &frame);
		bool FindAndReadFrame(Frame &frame);
		bool FindAndReadFrameSequence(FrameSequence &sequence, size_t max_size);
	};

	bool GetWidthAndHeight(Frame &frame, size_t &width, size_t &height);

}

#endif // _DHFS_VOLUME_H