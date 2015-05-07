#include "File.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            File
////////////////////////////////////////////////////////////////////////////////////////////////////////

File::File(const String &_file_name, DWORD file_mode) :
	handle(INVALID_HANDLE_VALUE),
	file_name(_file_name),
	mode(file_mode)
{
}

File::~File()
{
	Close();
}

DWORD File::GetDesiredAccess(DWORD file_mode)
{
	DWORD desired_access = 0;

	if (file_mode & FileMode::kReadOnly) {
		desired_access |= GENERIC_READ;
	} else if (file_mode & FileMode::kReadWrite) {
		desired_access |= (GENERIC_READ | GENERIC_WRITE);
	} else {
		return -1;
	}

	return desired_access;
}

DWORD File::GetFlagsAndAttributes(DWORD file_mode)
{
	DWORD flags_attributes = FILE_ATTRIBUTE_NORMAL;

	if (file_mode & FileMode::kNoBuffering) {
		flags_attributes |= FILE_FLAG_NO_BUFFERING;
	}

	return flags_attributes;
}

BOOL File::Open(void)
{
	Close();

	DWORD desired_access = GetDesiredAccess(mode);
	DWORD flags_attributes = GetFlagsAndAttributes(mode);

	if (desired_access != -1) {
		handle = ::CreateFile(file_name.c_str(),					// lpFileName 
								desired_access,						// dwDesiredAccess 
								FILE_SHARE_READ | FILE_SHARE_WRITE,	// dwShareMode 
								NULL,								// lpSecurityAttributes 
								OPEN_EXISTING,						// dwCreationDisposition 
								flags_attributes,					// dwFlagsAndAttributes 
								NULL);								// hTemplateFile 
		if (handle != INVALID_HANDLE_VALUE) {			
			return TRUE;
		}
	}
	return FALSE;
}

BOOL File::Create(void)
{
	Close();

	DWORD desired_access = GetDesiredAccess(mode);
	DWORD flags_attributes = GetFlagsAndAttributes(mode);

	if (desired_access != -1) {
		handle = ::CreateFile(file_name.c_str(),					// lpFileName 
								desired_access,						// dwDesiredAccess 
								FILE_SHARE_READ | FILE_SHARE_WRITE,	// dwShareMode 
								NULL,								// lpSecurityAttributes 
								CREATE_ALWAYS,						// dwCreationDisposition 
								flags_attributes,					// dwFlagsAndAttributes 
								NULL);								// hTemplateFile 
		if (handle != INVALID_HANDLE_VALUE) {
			return TRUE;
		}
	}
	return FALSE;
}

void File::Close(void)
{
	::CloseHandle(handle);
	handle = INVALID_HANDLE_VALUE;
}

BOOL File::SetPointer(const ULONGLONG &offset)
{
	LARGE_INTEGER new_offset;
	new_offset.QuadPart = (LONGLONG)offset;
	return ::SetFilePointerEx(handle, new_offset, NULL, FILE_BEGIN);
}

BOOL File::Pointer(ULONGLONG &pointer)
{
	LARGE_INTEGER ret_pointer;
	LARGE_INTEGER zero_distance;
	zero_distance.QuadPart = 0;
	if (SetFilePointerEx(handle, zero_distance, &ret_pointer, FILE_CURRENT)) {
		pointer = ret_pointer.QuadPart;
		return TRUE;
	} else {
		return FALSE;
	}
}

DWORD File::Read(BYTE *buffer, DWORD size_in_bytes)
{
	DWORD rw = 0;
	if (ReadFile(handle, buffer, size_in_bytes, &rw, NULL)) {
		return rw;
	}
	return 0;
}

DWORD File::Write(const BYTE *buffer, DWORD size_in_bytes)
{
	DWORD rw = 0;
	if (WriteFile(handle, buffer, size_in_bytes, &rw, NULL)) {
		return rw;
	}
	return 0;
}

BOOL File::Size(ULONGLONG &file_size)
{
	LARGE_INTEGER size;
	file_size = 0;
	if (::GetFileSizeEx(handle, &size)) {
		file_size = size.QuadPart;
		return TRUE;
	}
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            ExFile
////////////////////////////////////////////////////////////////////////////////////////////////////////

ExFile::ExFile(const String &_file_name, DWORD file_mode /* = FileMode::kReadOnly */) :
	handle(INVALID_HANDLE_VALUE),
	file_name(_file_name),
	mode(file_mode)
{


}

ExFile::~ExFile(void)
{
	Close();
}

DWORD ExFile::GetDesiredAccess(DWORD file_mode)
{
	DWORD desired_access = 0;

	if (file_mode & FileMode::kReadOnly) {
		desired_access |= GENERIC_READ;
	}
	else if (file_mode & FileMode::kReadWrite) {
		desired_access |= (GENERIC_READ | GENERIC_WRITE);
	}
	else {
		return -1;
	}

	return desired_access;
}

DWORD ExFile::GetFlagsAndAttributes(DWORD file_mode)
{
	DWORD flags_attributes = FILE_ATTRIBUTE_NORMAL;

	if (file_mode & FileMode::kNoBuffering) {
		flags_attributes |= FILE_FLAG_NO_BUFFERING;
	}

	return flags_attributes;
}

void ExFile::Open(void)
{
	Close();

	DWORD desired_access = GetDesiredAccess(mode);
	DWORD flags_attributes = GetFlagsAndAttributes(mode);

	if (desired_access != -1) {
		handle = ::CreateFile(file_name.c_str(),					// lpFileName 
								desired_access,						// dwDesiredAccess 
								FILE_SHARE_READ | FILE_SHARE_WRITE,	// dwShareMode 
								NULL,								// lpSecurityAttributes 
								OPEN_EXISTING,						// dwCreationDisposition 
								flags_attributes,					// dwFlagsAndAttributes 
								NULL);								// hTemplateFile 
		if (handle == INVALID_HANDLE_VALUE) {
			throw FileException("ExFile::Open(): Не удалось открыть файл.");
		}
	} else {
		throw FileException("ExFile::Open(): Параметр задан неверно.");
	}
}

void ExFile::Create(void)
{
	Close();

	DWORD desired_access = GetDesiredAccess(mode);
	DWORD flags_attributes = GetFlagsAndAttributes(mode);

	if (desired_access != -1) {
		handle = ::CreateFile(file_name.c_str(),					// lpFileName 
								desired_access,						// dwDesiredAccess 
								FILE_SHARE_READ | FILE_SHARE_WRITE,	// dwShareMode 
								NULL,								// lpSecurityAttributes 
								CREATE_ALWAYS,						// dwCreationDisposition 
								flags_attributes,					// dwFlagsAndAttributes 
								NULL);								// hTemplateFile 
		if (handle == INVALID_HANDLE_VALUE) {
			throw FileException("ExFile::Create(): Не удалось создать файл.");
		}
	}
	else {
		throw FileException("ExFile::Create(): Параметр задан неверно.");
	}
}

void ExFile::Close(void)
{
	::CloseHandle(handle);
	handle = INVALID_HANDLE_VALUE;
}

void ExFile::SetPointer(const ULONGLONG &offset)
{
	LARGE_INTEGER new_offset;
	new_offset.QuadPart = (LONGLONG)offset;
	if (!::SetFilePointerEx(handle, new_offset, NULL, FILE_BEGIN)) {
		throw FileException("Не удалось установить файловый указатель.");
	}
}

void ExFile::Pointer(ULONGLONG &pointer)
{
	LARGE_INTEGER ret_pointer;
	LARGE_INTEGER zero_distance;
	zero_distance.QuadPart = 0;
	if (SetFilePointerEx(handle, zero_distance, &ret_pointer, FILE_CURRENT)) {
		pointer = ret_pointer.QuadPart;
	} else {
		throw FileException("ExFile::Pointer(): Ошибка.");
	}
}