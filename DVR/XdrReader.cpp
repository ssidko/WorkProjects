#include "XdrReader.h"
#include <assert.h>
#include <stdexcept>

#include <intrin.h> // for byte swap; exclusive for Visual C++

XdrReader::XdrReader(uint8_t * buffer, size_t size) : pos(buffer), avail_size(size)
{
	assert(buffer && size);
}

uint32_t XdrReader::UInt32(void)
{
	uint32_t val = 0;
	if (avail_size >= 4) {
		val = (uint32_t)_byteswap_ulong(*(unsigned long *)pos);
		pos += 4;
		avail_size -= 4;
	} else {
		throw std::out_of_range("End of buffer");
	}
	return val;
}

int64_t XdrReader::Int64(void)
{
	int64_t val = 0;
	if (avail_size >= 8) {
		val = (int64_t)_byteswap_uint64(*(unsigned long long *)pos);
		pos += 8;
		avail_size -= 8;
	} else {
		throw std::out_of_range("End of buffer");
	}
	return val;
}

std::string XdrReader::String(void)
{
	auto str_size = UInt32();
	auto aligned_size = ALIGN4(str_size);
	if (avail_size >= aligned_size) {
		std::string val((const char *)pos, str_size);		
		pos += aligned_size;
		avail_size -= aligned_size;
		return val;
	} 
	throw std::out_of_range("End of buffer");
}
