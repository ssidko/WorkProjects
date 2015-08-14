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
	virtual BOOL Open(void);
	virtual void Close(void);
	virtual const DWORD &BlockSize(void);
	virtual const ULONGLONG &BlocksCount(void);
	virtual LONGLONG Pointer(void);
	virtual BOOL SetPointer(const LONGLONG &new_pointer);
	virtual BOOL ReadBlocks(void *buffer, DWORD size_in_blocks, DWORD *blocks_readed = NULL);
	virtual BOOL WriteBlocks(void *buffer, DWORD size_in_blocks, DWORD *blocks_readed = NULL);

	/*--== Extended functionality ==--*/
};

MY_LIB_NAMESPACE_END

#endif // _PHYSICAL_DRIVE_H