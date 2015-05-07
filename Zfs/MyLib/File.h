#ifndef _FILE_H
#define _FILE_H

#include <windows.h>
#include "MString.h"

class iFile 
{
public:
	virtual ~iFile(void) {};
	virtual BOOL Open(void) = 0;
	virtual BOOL Create(void) = 0;
	virtual void Close(void) = 0;
	virtual BOOL SetPointer(const ULONGLONG &offset) = 0;
	virtual BOOL Pointer(ULONGLONG &pointer) = 0;
	virtual DWORD Read(BYTE *buffer, DWORD size_in_bytes) = 0;
	virtual DWORD Write(const BYTE *buffer, DWORD size_in_bytes) = 0;
};

enum FileMode {
	kReadOnly		= 1 << 1,
	kReadWrite		= 1 << 2,
	kNoBuffering	= 1 << 3, 
};

class File : public iFile
{
private:
	HANDLE handle;
	String file_name;
	DWORD mode;

	DWORD GetDesiredAccess(DWORD file_mode);
	DWORD GetFlagsAndAttributes(DWORD file_mode);
public:
	File(const String &_file_name, DWORD file_mode = FileMode::kReadOnly);
	~File();
	virtual BOOL Open(void);
	virtual BOOL Create(void);
	virtual void Close(void);
	virtual BOOL SetPointer(const ULONGLONG &offset);
	virtual BOOL Pointer(ULONGLONG &pointer);
	virtual DWORD Read(BYTE *buffer, DWORD size_in_bytes);
	virtual DWORD Write(const BYTE *buffer, DWORD size_in_bytes);
	BOOL Size(ULONGLONG &file_size);
};

//
// Генерит исключение в случае ошибки
//
class iExFile
{
public:
	virtual ~iExFile() {};
	virtual void Open(void) = 0;
	virtual void Create(void) = 0;
	virtual void Close(void) = 0;
	virtual void SetPointer(const ULONGLONG &offset) = 0;
	virtual void Pointer(ULONGLONG &pointer) = 0;
	virtual ULONGLONG Pointer(void) = 0;
	virtual DWORD Read(BYTE *buffer, DWORD size_in_bytes) = 0;
	virtual DWORD Write(const BYTE *buffer, DWORD size_in_bytes) = 0;
};

class FileException : public std::exception
{
private:
	std::string error_message;
public:
	FileException(const std::string &_error_message) : error_message(_error_message) {}
	~FileException(void) {}
	virtual const char * what(void) const { return error_message.c_str(); }
};

class ExFile : public iExFile
{
private:
	HANDLE handle;
	String file_name;
	DWORD mode;

	DWORD GetDesiredAccess(DWORD file_mode);
	DWORD GetFlagsAndAttributes(DWORD file_mode);
public:
	ExFile(const String &_file_name, DWORD file_mode = FileMode::kReadOnly);
	~ExFile(void);
	virtual void Open(void);
	virtual void Create(void);
	virtual void Close(void);
	virtual void SetPointer(const ULONGLONG &offset);
	virtual void Pointer(ULONGLONG &pointer);
	//virtual ULONGLONG Pointer(void);
	//virtual DWORD Read(BYTE *buffer, DWORD size_in_bytes);
	//virtual DWORD Write(const BYTE *buffer, DWORD size_in_bytes);
	//void Size(ULONGLONG &file_size);
};

#endif // _FILE_H