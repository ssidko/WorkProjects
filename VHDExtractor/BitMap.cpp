#include "BitMap.h"


BitMap::BitMap(BYTE *bit_buffer, DWORD bits_count) : buffer(NULL), size(bits_count)
{
	DWORD size_in_bytes = bits_count / 8;
	if (bits_count % 8) {
		size_in_bytes++;
	}
	buffer = (DWORD *)new BYTE[size_in_bytes];
	memset(buffer, 0x00, size_in_bytes);
	memcpy((void *)buffer, (void *)bit_buffer, size_in_bytes);
}


BitMap::~BitMap(void)
{
}
