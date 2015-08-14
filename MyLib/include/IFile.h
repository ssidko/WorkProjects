#ifndef _IFILE_H
#define _IFILE_H

#include "MyLib.h"

#define	INVALID_FILE_POINTER			((LONGLONG)-1LL)

MY_LIB_NAMESPACE_START

typedef enum {
	kReadOnly		= 1 << 1,
	kWriteOnly		= 1 << 2,
	kReadWrite		= kReadOnly | kWriteOnly,
} FileMode;

class iFile
{
public:
	virtual ~iFile(void) {}
	virtual BOOL Opened(void) = 0;
	virtual BOOL Open(void) = 0;
	virtual BOOL Open(DWORD file_mode) = 0;
	virtual BOOL Create(void) = 0;
	virtual BOOL Create(DWORD file_mode) = 0;
	virtual void Close(void) = 0;
	virtual LONGLONG Pointer(void) = 0;
	virtual BOOL SetPointer(const LONGLONG &new_pointer) = 0;
	virtual BOOL Read(void *buffer, DWORD size, DWORD *readed = NULL) = 0;
	virtual BOOL Write(void *buffer, DWORD size, DWORD *written = NULL) = 0;
	virtual LONGLONG Size(void) = 0;
};

MY_LIB_NAMESPACE_END

#endif // _IFILE_H