#ifndef BITMAP_H
#define	BITMAP_H

#include <windows.h>

class BitMap
{
private:
	DWORD *buffer;
	DWORD size;
	
public:
	explicit BitMap(BYTE *bit_buffer, DWORD bits_count);
	~BitMap(void);
};

#endif

