#ifndef _DCH264_ONDISK_H
#define _DCH264_ONDISK_H

#include <windows.h>
#include "dvr.h"

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
					} subtype_0;

					struct {
						enum { kHeaderSize = 24 };
					} subtype_1;
				};
			} dc;

			struct FRAME_WB {
				enum { kHeaderSize = 8 };
				uint16_t size_1;
				uint16_t size_2;
			} wb;
		};

		bool IsValid(void);
		size_t HeaderSize(void);
		size_t PayloadSize(void);
		size_t FrameSize(void);
		uint8_t *Payload(void);
	};

#pragma pack(pop)

}

#endif // _DCH264_ONDISK_H
