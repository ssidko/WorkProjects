#pragma once
#ifndef _BITARRAY_H
#define _BIT_ARRAY_H

#include <stdint.h>
#include <vector>

class BitArray
{
public:
	BitArray();
	~BitArray();

	uint8_t *Data(void);
	size_t SizeInBytes(void);

private:
	std::vector<uint8_t> buff;
};

#endif // _BIT_ARRAY_H

