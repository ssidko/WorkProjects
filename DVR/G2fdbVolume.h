#ifndef _G2FDB_VOLUME_H
#define _G2FDB_VOLUME_H

#include "windows.h"
#include "g2fdb_ondisk.h"
#include "BufferedFile.h"
#include <vector>

namespace G2FDB
{

	class G2fdbVolume
	{
	private:
		BufferedFile io;
	public:
		G2fdbVolume(const std::string &volume_file);
		~G2fdbVolume();
		bool Open(void);
		bool SetPointer(LONGLONG &offset);
		bool ReadFrame(std::vector<BYTE> &buffer);
	};

}

#endif // _G2FDB_VOLUME_H

