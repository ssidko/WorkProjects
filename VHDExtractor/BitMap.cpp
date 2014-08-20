#include "BitMap.h"
#include <assert.h>

// class BitMapLE

BitMapLE::BitMapLE(char *bit_buffer, DWORD bits_count) : buffer(NULL), size(bits_count)
{
	DWORD size_in_bytes = bits_count / 8;
	if (bits_count % 8) {
		size_in_bytes++;
	}
	buffer = new char[size_in_bytes];
	memcpy((void *)buffer, (void *)bit_buffer, size_in_bytes);
}

BitMapLE::~BitMapLE(void)
{
	if (buffer) {
		delete[] buffer;
	}
}

bool BitMapLE::TestBit(const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	return ((buffer[zero_based_bit_index/8] & (1 << (zero_based_bit_index%8))) > 0);
}

void BitMapLE::SetBit(const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	buffer[zero_based_bit_index/8] = (buffer[zero_based_bit_index/8] | (1 << (zero_based_bit_index%8)));
}

void BitMapLE::ClearBit(const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	buffer[zero_based_bit_index/8] = (buffer[zero_based_bit_index/8] & ~(1 << (zero_based_bit_index%8)));
}

bool BitMapLE::operator[](const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	return ((buffer[zero_based_bit_index/8] & (1 << (zero_based_bit_index%8))) > 0);
}

DWORD BitMapLE::Size(void)
{
	return size;
}

DWORD BitMapLE::OneCount( void )
{
	DWORD counter = 0;
	for (DWORD i = 0; i < size; i++) {
		if (TestBit(i)) {
			counter++;
		}
	}
	return counter;
}

DWORD BitMapLE::ZeroCount( void )
{
	DWORD counter = 0;
	for (DWORD i = 0; i < size; i++) {
		if (!TestBit(i)) {
			counter++;
		}
	}
	return counter;
}

// class BitMapBE

BitMapBE::BitMapBE(char *bit_buffer, DWORD bits_count) : buffer(NULL), size(bits_count)
{
	DWORD size_in_bytes = bits_count / 8;
	if (bits_count % 8) {
		size_in_bytes++;
	}
	buffer = new char[size_in_bytes];
	memcpy((void *)buffer, (void *)bit_buffer, size_in_bytes);
}

BitMapBE::~BitMapBE(void)
{
	if (buffer) {
		delete[] buffer;
	}
}

bool BitMapBE::TestBit(const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	return ((buffer[zero_based_bit_index/8] & (1 << (7 - zero_based_bit_index%8))) > 0);
}

void BitMapBE::SetBit(const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	buffer[zero_based_bit_index/8] = (buffer[zero_based_bit_index/8] | (1 << (7 - zero_based_bit_index%8)));
}

void BitMapBE::ClearBit(const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	buffer[zero_based_bit_index/8] = (buffer[zero_based_bit_index/8] & ~(1 << (7 - zero_based_bit_index%8)));
}

bool BitMapBE::operator[](const DWORD &zero_based_bit_index)
{
	assert(zero_based_bit_index < size);
	return ((buffer[zero_based_bit_index/8] & (1 << (7 - zero_based_bit_index%8))) > 0);
}

DWORD BitMapBE::Size(void)
{
	return size;
}

DWORD BitMapBE::OneCount( void )
{
	DWORD counter = 0;
	for (DWORD i = 0; i < size; i++) {
		if (TestBit(i)) {
			counter++;
		}
	}
	return counter;
}

DWORD BitMapBE::ZeroCount( void )
{
	DWORD counter = 0;
	for (DWORD i = 0; i < size; i++) {
		if (!TestBit(i)) {
			counter++;
		}
	}
	return counter;
}
