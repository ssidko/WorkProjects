#include "nvpair.h"
#include "XdrReader.h"
#include <assert.h>

#include <intrin.h> // for byte swap; exclusive for Visual C++

#define MAX_VALUE_SIZE					2147483647
#define	XDR_NVPAIR_MIN_SIZE				((size_t)(5 * 4))


bool DecodeXdrNVPair(uint8_t *buff, size_t size)
{
	assert(buff && size);

	if (size < XDR_NVPAIR_MIN_SIZE) {
		return false;
	}

	XdrReader xdr(buff, size);

	try {

		size_t encode_size = xdr.UInt32();
		size_t decode_size = xdr.UInt32();
		size_t name_size = xdr.UInt32();

		std::string name = xdr.String();
		uint32_t value_type = xdr.UInt32();
	
	
	} catch (const std::out_of_range &ex) {
		return false;	
	}

	return false;
}

bool DecodeXdrNVList(uint8_t *buff, size_t size)
{
	return false;
}