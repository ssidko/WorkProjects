#ifndef _DBF_H
#define _DBF_H

#include <stdint.h>

namespace dbf
{

	struct header_t {
		uint8_t flags;
		uint8_t year;				// 1900 + HEADER::year
		uint8_t month;
		uint8_t day;
		uint32_t records_count;
		uint16_t header_size;
		uint16_t record_size;
		uint16_t reserved_1;		// 0x00
		uint8_t transaction_incomplete_flag;
		uint8_t encryption_flag;
		uint8_t reserved_2[12];
		uint8_t index_flag;
		uint8_t reserved_3[3];
	};

	struct field_descriptor_t {

	};


}

#endif // _DBF_H
