#include "File.h"

File::File(const String &_file_name, FileMode _mode) :
	handle(INVALID_HANDLE_VALUE),
	file_name(_file_name),
	mode(_mode)
{
}

File::~File()
{
}

DWORD File::GetDesiredAccess(FileMode open_mode)
{
	DWORD desired_access = 0;

	if (open_mode && FileMode::kReadOnly) {
		desired_access |= GENERIC_READ;
	} else if (open_mode && FileMode::kReadWrite) {
		desired_access |= (GENERIC_READ | GENERIC_WRITE);
	} else {
		return -1;
	}

	return desired_access;
}

DWORD File::GetFlagsAndAttributes(FileMode open_mode)
{
	DWORD flags_attributes = FILE_ATTRIBUTE_NORMAL;

	if (open_mode && FileMode::kNoBuffering) {
		flags_attributes |= FILE_FLAG_NO_BUFFERING;
	}

	return flags_attributes;
}

BOOL File::Open(void)
{
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