#include "nvpair.h"
#include <assert.h>

#include <intrin.h> // for byte swap; exclusive for Visual C++

#define MAX_VALUE_SIZE			2147483647

bool DecodeXdrNVPair(uint8_t *buff, size_t size)
{
	assert(buff);
	assert(size);

	uint8_t *pos = buff;

	size_t encode_size = _byteswap_ulong(((xdr_nvpair *)buff)->encoded_size);
	size_t decode_size = _byteswap_ulong(((xdr_nvpair *)buff)->decoded_size);
	size_t name_size = _byteswap_ulong(((xdr_nvpair *)buff)->name_size);


	return false;
}

bool DecodeXdrNVList(uint8_t *buff, size_t size)
{
	return false;
}