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
	DWORD offset;
	DWORD readed;
public:
	BufferedFile(const std::string &file_name, DWORD buffer_size = 0);
	~BufferedFile(void);
	bool Open(void);

	LONGLONG Pointer(void);
	void Pointer(LONGLONG &pointer);
	bool SetPointer(const LONGLONG &new_pointer);
	DWORD Read(void *buff, DWORD count);
};

#endif /*_BUFFERED_FILE_H*/

