#ifndef _IFILE_H
#define _IFILE_H

#include "MyLib.h"

#define	INVALID_FILE_POINTER			((LONGLONG)-1LL)

MY_LIB_NAMESPACE_START

class iFile
{
public:
	typedef enum {
		kReadOnly		= 1 << 1,
		kWriteOnly		= 1 << 2,
		kReadWrite		= kReadOnly | kWriteOnly,
	} FileMode;

	virtual ~iFile(void) {}

	virtual BOOL IsOpen(void) = 0;
	virtual BOOL Open(DWORD file_mode) = 0;
	virtual BOOL Create(DWORD file_mode) = 0;
	virtual void Close(void) = 0;

	virtual LONGLONG Pointer(void) = 0;
	virtual BOOL SetPointer(const LONGLONG &new_pointer) = 0;

	virtual BOOL Read(void *buffer, DWORD size, DWORD &readed) = 0;
	virtual BOOL Write(void *buffer, DWORD size, DWORD &written) = 0;

	virtual LONGLONG Size(void) = 0;
};

MY_LIB_NAMESPACE_END

#endif // _IFILE_H