#include "BufferedFile.h"
#include <assert.h>

#define MEM_PAGE_SIZE					((DWORD)4096)
#define DEFAULT_BUFFER_SIZE				MEM_PAGE_SIZE
#define BUFFER_SIZE_MASK				((DWORD)0x00000FFF)
#define FILE_POINTER_MASK				((LONGLONG)0x0000000000000FFF)

BufferedFile::BufferedFile(const std::string &file_name, DWORD buffer_size) :
	name(file_name),
	io_pointer(0),
	offset(0),
	readed(0)
{
	if (buffer_size) {
		/*Размер буфера должен быть кратным MEM_PAGE_SIZE*/
		assert(!(buffer_size & BUFFER_SIZE_MASK));
	} else {
		buffer_size = DEFAULT_BUFFER_SIZE;
	}
	buffer.resize(buffer_size);
}

BufferedFile::~BufferedFile(void)
{
}

BOOL BufferedFile::SyncBuffer(void)
{
	if (readed == 0) {
		readed = io.Read(&buffer[0], buffer.size());
		if (readed == 0) {
			return FALSE;
		}
	}
	return TRUE;
}

BOOL BufferedFile::Open(void)
{
	io_pointer = (0);
	offset = (0);
	readed = 0;
	return io.Open(name.c_str());
}

LONGLONG BufferedFile::Pointer(void)
{
	return ((LONGLONG)io_pointer + offset);
}

void BufferedFile::Pointer(LONGLONG &pointer)
{
	pointer = ((LONGLONG)io_pointer + offset);
}

BOOL BufferedFile::SetPointer(const LONGLONG &new_pointer)
{
	if ((new_pointer < io_pointer) || (new_pointer >= (io_pointer + buffer.size()))) {
		io_pointer = new_pointer & ~FILE_POINTER_MASK;
		offset = (DWORD)(new_pointer - io_pointer);
		readed = 0;
		return io.SetPointer(io_pointer);
	} else {
		offset = (DWORD)(new_pointer - io_pointer);
	}
	return true;
}

DWORD BufferedFile::Read(void *buff, DWORD count)
{
	assert(buff);

	if (readed == 0) {
		readed = io.Read(&buffer[0], buffer.size());
		if (readed == 0) {
			return 0;
		}
	}
	if (offset < readed) {
		DWORD rd = 0;
		if (count <= (readed - offset)) {
			rd  = count;
			memcpy(buff, (void *)&buffer[offset], rd);
			offset += rd;
		} else {
			rd = (readed - offset);
			memcpy(buff, (void *)&buffer[offset], rd);
			offset += rd;
			if (readed == buffer.size()) {
				SetPointer(io_pointer + buffer.size());
				rd += Read(&((BYTE *)buff)[rd], count - rd);
			}
		}
		if (offset >= buffer.size()) {
			SetPointer(io_pointer + buffer.size());
		}
		return rd;
	} 
	return 0;
}

LONGLONG BufferedFile::Find(BYTE *byte_string, DWORD length)
{
	assert(byte_string);
	assert(length);

	DWORD rw = 0;
	LONGLONG old_pointer = Pointer();
	LONGLONG pointer = old_pointer;
	BYTE *buff = new BYTE[DEFAULT_BUFFER_SIZE];
	BYTE *substring = NULL; 
	if (buff && (length <= DEFAULT_BUFFER_SIZE)) {
		while (rw = Read(buff, DEFAULT_BUFFER_SIZE)) {
			if (rw >= length) {
				for (int i = 0; (i + length) < rw; ++i) {
					substring = &buff[i];
					for (int j = 0; j < length; ++j) {
						if (substring[j] != byte_string[j]) {
							break;
						}
						if (j >= (length - 1)) {
							pointer += i;
							SetPointer(pointer);
							delete[] buff;
							return (pointer);
						}
					}
				}
				pointer += (rw - length + 1);
				SetPointer(pointer);
			}
		}
		DWORD err = 0;
		err = ::GetLastError();
		if (err) {
			int x = 0;
		}
	}

	SetPointer(old_pointer);
	delete[] buff;
	return ((LONGLONG)-1);
}
