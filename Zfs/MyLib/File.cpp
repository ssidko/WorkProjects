#include "File.h"

File::File(const String &_file_name, FileMode _mode, const ULONGLONG &offset_in_bytes) :
	handle(INVALID_HANDLE_VALUE),
	file_name(_file_name),
	mode(_mode)
{
}


File::~File()
{
}

DWORD GetDeisiredAccess(FileMode open_mode)
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

BOOL File::Open(void)
{
	DWORD desired_access = GetDesiredAccess(mode);

	handle = ::CreateFile(file_name.c_str(),
		desired_access,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	return FALSE;
}