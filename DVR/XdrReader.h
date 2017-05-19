#ifndef _XDR_READER_H
#define _XDR_READER_H

#include <stdint.h>
#include <string>

class XdrReader
{
private:
	uint8_t *pos;
	size_t avail_size;
public:
	XdrReader(uint8_t *buffer, size_t size);

	//
	// throw std::out_of_range("End of buffer");
	//
	uint32_t UInt32(void);
	int64_t Int64(void);
	std::string String(void);
};

#endif // _XDR_READER_H

