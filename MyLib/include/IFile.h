#ifndef _IFILE_H
#define _IFILE_H

#include <windows.h>

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
	virtual BOOL SetPointer(LONGLONG new_pointer) = 0;

	virtual BOOL Read(void *buffer, DWORD size, DWORD &readed) = 0;
	virtual BOOL Write(void *buffer, DWORD size, DWORD &written) = 0;

	virtual LONGLONG Size(void);
};

#endif // _IFILE_H