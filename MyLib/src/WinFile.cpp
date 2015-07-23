#include "stdafx.h"
#include "WinFile.h"

MyLib::WinFile::WinFile(const TCHAR *file_name, DWORD file_mode) : 
	opened(FALSE),
	handle(INVALID_HANDLE_VALUE),
	name(file_name)
{
}

MyLib::WinFile::~WinFile(void)
{
	Close();
}

BOOL MyLib::WinFile::WinCreateFile(DWORD desired_access, DWORD share_mode, LPSECURITY_ATTRIBUTES security_attributes, DWORD creation_disposition, DWORD flags_and_attributes, HANDLE template_file)
{
	Close();
	handle = ::CreateFile(name.data(), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file);
	if (handle != INVALID_HANDLE_VALUE) {
		opened = TRUE;
	}
	else {
		last_error.Update();
	}
	return opened;
}

void MyLib::WinFile::Close(void)
{
	if (handle != INVALID_HANDLE_VALUE) {
		::CloseHandle(handle);
	}
	opened = FALSE;
	handle = INVALID_HANDLE_VALUE;
}
