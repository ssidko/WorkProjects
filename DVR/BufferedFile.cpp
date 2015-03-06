#include "BufferedFile.h"
#include <assert.h>

#define MEM_PAGE_SIZE					((DWORD)4096)
#define DEFAULT_BUFFER_SIZE				MEM_PAGE_SIZE
#define BUFFER_SIZE_MASK				((DWORD)0xFFFFF000)
#define FILE_POINTER_MASK				((LONGLONG)0xFFFFFFFFFFFFF000)

BufferedFile::BufferedFile(const std::string &file_name, DWORD buffer_size) :
	name(file_name),
	io_pointer(0),
	offset(0),
	readed(0)
{
	if (buffer_size) {
		/*Размер буфера должен быть кратным MEM_PAGE_SIZE*/
		assert(!(buffer_size && BUFFER_SIZE_MASK));
	} else {
		buffer_size = DEFAULT_BUFFER_SIZE;
	}
	buffer.resize(buffer_size);
}

BufferedFile::~BufferedFile(void)
{
}

bool BufferedFile::Open(void)
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

bool BufferedFile::SetPointer(const LONGLONG &new_pointer)
{
	if ((new_pointer < io_pointer) || (new_pointer >= (io_pointer + buffer.size()))) {
		io_pointer = new_pointer & FILE_POINTER_MASK;
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
	if (readed == 0) {
		readed = io.Read(&buffer[0], buffer.size());
		if (!readed) {
			return 0;
		}
	}
	if (readed > offset) {
		if (count <= (readed - offset)) {
			memcpy(buff, (void *)&buffer[offset], count);
			offset += count;
			return count;
		} else {
			DWORD rd = (readed - offset);
			memcpy(buff, (void *)&buffer[offset], rd);
			offset += rd;
			if (readed == buffer.size()) {
				SetPointer(io_pointer + buffer.size());
				rd += Read(&((BYTE *)buff)[rd], count - rd);
			}
			return rd;
		}
	}
	return 0;
}
