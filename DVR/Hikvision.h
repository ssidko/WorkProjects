#ifndef _HIKVISION_H
#define _HIKVISION_H

#include <string>
#include "Timestamp.h"
#include "BufferedFile.h"

namespace HIKV
{
#pragma pack(push)
#pragma pack(1)

	typedef struct _FRAME_HEADER {
		BYTE signature;			// {0x00, 0x00, 0x01}
		BYTE type;
		WORD data_size;
		bool IsFixedDataSize(void) { return type == 0xBA; }
		DWORD DataSize(void);
	} FRAME_HEADER;

#pragma pack(pop)

	class HikVolume
	{
	private:
		BufferedFile io;
	public: 
		HikVolume(const std::string &file_name) : io(file_name, 256*512) {}
		~HikVolume(void) {}

		bool Open(void) { return io.Open(); }

		void SetPointer(const LONGLONG &offset) { io.SetPointer(offset); }
		LONGLONG GoToNextFrame(void);
		bool ReadFrame(std::vector<BYTE> &buffer);
	};

	int StartRecovering(const std::string &dhfs_volume, const std::string &out_directory, const Timestamp &start_time, const Timestamp &end_time);

}



#endif // _HIKVISION_H

