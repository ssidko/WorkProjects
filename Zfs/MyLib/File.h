#ifndef _FILE_H
#define _FILE_H

#include <windows.h>
#include "String.h"

class IFile 
{
public:
	virtual ~IFile(void) = 0;
	virtual BOOL Open(void) = 0;
	virtual BOOL Create(void) = 0;
	virtual void Close(void) = 0;
	virtual BOOL SetPointer(const ULONGLONG &offset) = 0;
	virtual DWORD Read(const BYTE *buffer, DWORD size_in_bytes) = 0;
	virtual DWORD Write(const BYTE *buffer, DWORD size_in_bytes) = 0;
};

enum FileMode {
	kReadOnly		= 1 << 1,
	kReadWrite		= 1 << 2,
	kNoBuffering	= 1 << 3, 
};

class File : public IFile
{
private:
	HANDLE handle;
	String file_name;
	FileMode mode;
	DWORD GetDesiredAccess(FileMode open_mode);
	DWORD GetFlagsAndAttributes(FileMode open_mode);
public:
	File(const String &_file_name, FileMode _mode);
	~File();

	virtual BOOL Open(void);
	virtual BOOL Create(void);
	virtual void Close(void);
	virtual BOOL SetPointer(const ULONGLONG &offset);
	virtual DWORD Read(const BYTE *buffer, DWORD size_in_bytes);
	virtual DWORD Write(const BYTE *buffer, DWORD size_in_bytes);
};

#endif // _FILE_H