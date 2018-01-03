#pragma once
#include <stdint.h>

class ByteStream
{
public:
	ByteStream(uint8_t *buffer, size_t size);
	~ByteStream();
	size_t Read(uint8_t *destination, size_t count);
	size_t Remain(void) { return remain; }
private:
	uint8_t *pos;
	size_t remain;
};
