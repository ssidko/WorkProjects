#pragma once
#ifndef _BITARRAY_H
#define _BIT_ARRAY_H

#include <stdint.h>
#include <vector>

class BitArray
{
public:
	BitArray(std::vector<uint8_t>&& buffer, size_t number_of_bits);
	~BitArray();

	bool GetBit(size_t index);
	void SetBit(size_t index);
	void ClearBit(size_t index);

private:
	std::vector<uint8_t> buff;
	size_t size;
};

#endif // _BIT_ARRAY_H

