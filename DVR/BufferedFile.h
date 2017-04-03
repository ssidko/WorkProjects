#pragma once
#ifndef _BUFFERED_FILE_H
#define _BUFFERED_FILE_H

#include <windows.h>
#include <string>
#include <vector>
#include "File.h"

class BufferedFile
{
private:
	std::string name;
	std::vector<BYTE> buffer;
	W32Lib::FileEx io;
	LONGLONG io_pointer;
	LONGLONG io_size;
	DWORD offset;
	DWORD readed;
	bool UpdateBuffer(void);
public:
	BufferedFile(const std::string &file_name, size_t buffer_size = 0, LONGLONG size = 0);
	~BufferedFile(void);
	BOOL Open(void);
	void Close(void);
	LONGLONG Pointer(void);
	void Pointer(LONGLONG &pointer);
	BOOL SetPointer(const LONGLONG &new_pointer);
	DWORD Read(void *buff, DWORD count);
	LONGLONG Find(BYTE *byte_string, DWORD length);
	bool Find(BYTE *byte_string, DWORD length, LONGLONG &offset);
};

#endif /*_BUFFERED_FILE_H*/

