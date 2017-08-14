#ifndef _DCH264_ONDISK_H
#define _DCH264_ONDISK_H

#include <windows.h>
#include <string>
#include "dvr.h"
#include "BufferedFile.h"

namespace dcH264
{
#pragma pack(push, 1)

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
		uint8_t camera;							// ASCII '0' - '9'
		uint8_t subtype;						// ASCII '0' - '9'
		uint16_t frame_type;					// 'dc', 'wb' ...

		union {
			struct FRAME_DC {
				uint32_t signature;				// 'H264'
				uint32_t payload_size;
				uint8_t unknown[12];

				union {
					struct {
						enum { kHeaderSize = 32 };
						TIMESTAMP timestamp;
						uint8_t padding[2];
						uint8_t payload[1];
					} subtype_0;

					struct {
						enum { kHeaderSize = 24 };
						uint8_t payload[1];
					} subtype_1;
				};
			} dc;

			struct FRAME_WB {
				enum { kHeaderSize = 20 };
				uint8_t size_1;
				uint8_t size_2;
			} wb;
		};		
	};

#pragma pack(pop)

	class FrameReader
	{
	public:

	};

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

	int main(void);
}

#endif // _DCH264_ONDISK_H
