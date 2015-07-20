#ifndef _BLOCK_DEVICE_H
#define _BLOCK_DEVICE_H

class IBlockDevice
{
public:
	virtual ~IBlockDevice(void) {};

	virtual bool Open(void) = 0;
	virtual void Close(void) = 0;

	virtual size_t BlockSize(void) = 0;
	virtual long long BlocksCount(void) = 0;

	virtual bool SetPointer(const long long  &offset_in_blocks) = 0;
	virtual long long Pointer(void) = 0;

	virtual size_t ReadBlock(void *block_buffer, size_t blocks_count) = 0;
	virtual size_t WriteBlock(void *block_buffer, size_t blocks_count) = 0;
};

#endif // _BLOCK_DEVICE_H