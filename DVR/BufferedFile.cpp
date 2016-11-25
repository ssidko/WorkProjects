#include "BufferedFile.h"
#include <assert.h>

#define MEM_PAGE_SIZE					((DWORD)4096)
#define DEFAULT_BUFFER_SIZE				MEM_PAGE_SIZE
#define BUFFER_SIZE_MASK				((DWORD)0x00000FFF)
#define FILE_POINTER_MASK				((LONGLONG)0x0000000000000FFF)

BufferedFile::BufferedFile(const std::string &file_name, size_t buffer_size, LONGLONG size) :
	name(file_name),
	io_pointer(0),
	io_size(size),
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

bool BufferedFile::UpdateBuffer(void)
{
	size_t buffer_size = buffer.size();
	size_t to_read = 0;

	if (io_size) {
		LONGLONG curr_pointer;
		io.GetPointer(&curr_pointer);
		if (curr_pointer < io_size) {
			to_read = (curr_pointer + buffer_size) <= io_size ? buffer_size : (size_t)(io_size - curr_pointer);
		}
	} else {
		to_read = buffer_size;
	}

	if (to_read) {
		readed = io.Read(&buffer[0], to_read);
		if (readed) {
			return true;
		}		
	}

	return false;
}

DWORD BufferedFile::Read(void *buff, DWORD count)
{
	assert(buff);

	if (readed == 0) {
		if (!UpdateBuffer()) {
			return 0;
		}
	}

	if (offset < readed) {

		size_t available = readed - offset;
		size_t rd = (count <= available) ? count : available;
		std::memcpy(buff, &buffer[offset], rd);
		offset += rd;

		if (offset >= buffer.size()) {
			SetPointer(io_pointer + buffer.size());
			if (count > available) {
				rd += Read(&((BYTE *)buff)[rd], count - rd);
			}
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

bool BufferedFile::Find(BYTE *byte_string, DWORD length, LONGLONG &offset)
{
	assert(byte_string);
	assert(length);

	std::vector<BYTE> buff(DEFAULT_BUFFER_SIZE, 0x00);

	size_t rw = 0;
	LONGLONG pointer = Pointer();
	while ((rw = Read(&buff[0], buff.size())) >= length) {
	
		for (int i = 0; i + length <= rw; i++) {		
			if (0x00 == std::memcmp(&buff[i], byte_string, length)) {
			
				offset = pointer + i;
				SetPointer(offset);
				return true;
			}		
		}

		pointer += rw - (length - 1);
		SetPointer(pointer);
	}

	return false;
}
