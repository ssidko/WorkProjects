#ifndef _PHYSICAL_DRIVE_H
#define _PHYSICAL_DRIVE_H

#include "MyLib.h"
#include "iBlockDevice.h"

MY_LIB_NAMESPACE_START

class PhysicalDriveInfo;

class PhysicalDrive : public iBlockDevice
{
private:
	std::basic_string<TCHAR> name;

public:
	PhysicalDrive();
	~PhysicalDrive();

	/*--== iBlockDevice interface ==--*/
	virtual BOOL Open(void) = 0;
	virtual void Close(void) = 0;
	virtual const DWORD &BlockSize(void) = 0;
	virtual const ULONGLONG &BlocksCount(void) = 0;
	virtual LONGLONG Pointer(void) = 0;
	virtual BOOL SetPointer(const LONGLONG &new_pointer) = 0;
	virtual BOOL ReadBlocks(void *buffer, DWORD size_in_blocks, DWORD *blocks_readed = NULL) = 0;
	virtual BOOL WriteBlocks(void *buffer, DWORD size_in_blocks, DWORD *blocks_readed = NULL) = 0;

	/*--== Extended functionality ==--*/
};

MY_LIB_NAMESPACE_END

#endif // _PHYSICAL_DRIVE_H