#include "stdafx.h"
#include "BitArray.h"
#include <assert.h>

BitArray::BitArray(std::vector<uint8_t>&& buffer, size_t number_of_bits) : buff(std::move(buffer)), size(number_of_bits)
{
	assert((number_of_bits >> 3) <= buffer.size());
}

BitArray::~BitArray()
{
}
