#include "BitMap.h"
#include <assert.h>

BitMap::BitMap(char *bit_buffer, DWORD bits_count) : buffer(NULL), size(bits_count)
{
	DWORD size_in_bytes = bits_count / 8;
	if (bits_count % 8) {
		size_in_bytes++;
	}
	buffer = new char[size_in_bytes];
	memcpy((void *)buffer, (void *)bit_buffer, size_in_bytes);
}

BitMap::~BitMap(void)
{
	if (buffer) {
		delete[] buffer;
	}
}

bool BitMap::TestBit(const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	return ((buffer[zero_based_bit_index/8] & (1 << (zero_based_bit_index%8))) > 0);
}

void BitMap::SetBit(const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	buffer[zero_based_bit_index/8] = (buffer[zero_based_bit_index/8] | (1 << (zero_based_bit_index%8)));
}

void BitMap::ClearBit(const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	buffer[zero_based_bit_index/8] = (buffer[zero_based_bit_index/8] & ~(1 << (zero_based_bit_index%8)));
}

bool BitMap::operator[](const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	return ((buffer[zero_based_bit_index/8] & (1 << (zero_based_bit_index%8))) > 0);
}

DWORD BitMap::Size(void)
{
	return size;
}

DWORD BitMap::OneCount( void )
{
	DWORD counter = 0;
	for (DWORD i = 0; i < size; i++) {
		if (TestBit(i)) {
			counter++;
		}
	}
	return counter;
}

DWORD BitMap::ZeroCount( void )
{
	DWORD counter = 0;
	for (DWORD i = 0; i < size; i++) {
		if (!TestBit(i)) {
			counter++;
		}
	}
	return counter;
}
