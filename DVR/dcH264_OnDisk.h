#ifndef _DCH264_ONDISK_H
#define _DCH264_ONDISK_H

#include <windows.h>
#include <string>
#include "dvr.h"
#include "BufferedFile.h"

namespace dcH264
{
#pragma pack (push)
#pragma pack (1)

	struct TIMESTAMP {
		BYTE year;
		BYTE month;
		BYTE day;
		BYTE hours;
		BYTE minutes;
		BYTE seconds;
		dvr::Timestamp Timestamp(void) { return dvr::Timestamp(2000 + year, month, day, hours, minutes, seconds); }
	};
		
	struct FRAME_HEADER {
		BYTE camera;				// ASCII '0' - '9'
		BYTE flags;					// ASCII '0' - '9'
		char type[2];				// 'dc', 'wb' ...
	};

	struct FRAME_H264 {
		FRAME_HEADER header;
		char sign[4];				// 'H264'

	};

#pragma pack(pop)

	class Volume
	{
	private:
		BufferedFile io;
	public:
		Volume(std::string &file_name);
		~Volume();

		bool Open(void);
		void Close(void);

		void SetPointer(const LONGLONG &new_pointer);
		LONGLONG Pointer(void);

		bool ReadFrame(dvr::Frame &frame);
	};
}

#endif // _DCH264_ONDISK_H
