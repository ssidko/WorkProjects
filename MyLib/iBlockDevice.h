#ifndef _IBLOCK_DEVICE_H
#define _IBLOCK_DEVICE_H

#include "MyLib.h"

MY_LIB_NAMESPACE_START

class iBlockDevice
{
public:
	virtual ~iBlockDevice() {}
	virtual BOOL Open(void) = 0;
	virtual void Close(void) = 0;
	virtual const DWORD &BlockSize(void) = 0;
	virtual const ULONGLONG &BlocksCount(void) = 0;
	virtual LONGLONG Pointer(void) = 0;
	virtual BOOL SetPointer(const LONGLONG &new_pointer) = 0;
	virtual BOOL ReadBlocks(void *buffer, DWORD size_in_blocks, DWORD *blocks_readed = NULL) = 0;
	virtual BOOL WriteBlocks(void *buffer, DWORD size_in_blocks, DWORD *blocks_readed = NULL) = 0;
};

MY_LIB_NAMESPACE_END

#endif // _IBLOCK_DEVICE_H

