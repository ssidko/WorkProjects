#include "stdafx.h"
#include <assert.h>
#include "ByteStream.h"


ByteStream::ByteStream(uint8_t *buffer, size_t size) : pos(buffer), remain(size)
{
	assert(buffer);
	assert(size);
}

ByteStream::~ByteStream()
{
}

size_t ByteStream::Read(uint8_t *destination, size_t count)
{
	assert(destination);
	size_t readed = 0; 
	while (remain && count) {
		*destination++ = *pos++;
		--remain;
		--count;
		++readed;
	}
	return readed;
}
