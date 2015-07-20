#ifndef _VOLUME_H
#define _VOLUME_H

#include "BlockDevice.h"
#include <string>
#include "File.h"

class Volume : public IBlockDevice
{
private:
	size_t block_size;
	long long volume_offset; // in bytes
	long long blocks_count;
	W32Lib::File io;
public:
	Volume(const std::string &io_file_name, const long long &offset_in_bytes, size_t io_block_size, const long long &blocks_count = 0);
	~Volume();

	virtual bool Open(void);
	virtual void Close(void);

	virtual size_t BlockSize(void);
	virtual long long BlocksCount(void);

	virtual bool SetPointer(const long long  &offset_in_blocks);
	virtual long long Pointer(void);

	virtual size_t ReadBlock(void *block_buffer, size_t blocks_count);
	virtual size_t WriteBlock(void *block_buffer, size_t blocks_count);
};

#endif // _VOLUME_H

