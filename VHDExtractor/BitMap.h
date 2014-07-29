#ifndef BITMAP_H
#define	BITMAP_H

#include <windows.h>

class BitMap
{
private:
	BYTE *buffer;
	DWORD size;	
public:
	explicit BitMap(BYTE *bit_buffer, DWORD bits_count);
	~BitMap(void);
	bool TestBit(const DWORD &zero_based_bit_index);
	void SetBit(const DWORD &zero_based_bit_index);
	void ClearBit(const DWORD &zero_based_bit_index);
	bool operator[](const DWORD &zero_based_bit_index);
	DWORD Size(void);
};

#endif

