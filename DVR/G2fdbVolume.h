#ifndef _G2FDB_VOLUME_H
#define _G2FDB_VOLUME_H

#include "windows.h"
#include "G2fdbOnDisk.h"
#include "BufferedFile.h"
#include <vector>

namespace G2FDB
{

	class G2fdbVolume
	{
	private:
		BufferedFile io;

		bool IsValidFrameHeader(const FRAME_HEADER &header);
	public:
		G2fdbVolume(const std::string &volume_file);
		~G2fdbVolume();
		bool Open(void);
		bool SetPointer(LONGLONG &offset);
		bool FindNextFrame();
		bool ReadFrame(std::vector<BYTE> &buffer);
	};

}

#endif // _G2FDB_VOLUME_H

