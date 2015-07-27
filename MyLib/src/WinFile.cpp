#include "stdafx.h"
#include "WinFile.h"

#define WINFILE_DEFAULT_VALUE__DESIRED_ACCESS				((DWORD)GENERIC_READ)
#define WINFILE_DEFAULT_VALUE__SHARE_MODE					((DWORD)FILE_SHARE_READ)
#define WINFILE_DEFAULT_VALUE__FLAGS_AND_ATTRIBUTES			((DWORD)FILE_ATTRIBUTE_NORMAL)

MyLib::WinFile::WinFile(const TCHAR *file_name) : 
	opened(FALSE),
	handle(INVALID_HANDLE_VALUE),
	name(file_name)
{
}

MyLib::WinFile::~WinFile(void)
{
	Close();
}

DWORD MyLib::WinFile::DesiredAccess(DWORD file_mode)
{
	DWORD desired_access = 0;
	if (file_mode && iFile::kReadOnly) {
		desired_access |= GENERIC_READ;
	}
	if (file_mode && iFile::kWriteOnly) {
		desired_access |= GENERIC_WRITE;
	}
	return desired_access;
}

BOOL MyLib::WinFile::WinCreateFile(DWORD desired_access, DWORD share_mode, LPSECURITY_ATTRIBUTES security_attributes, DWORD creation_disposition, DWORD flags_and_attributes, HANDLE template_file)
{
	Close();
	handle = ::CreateFile(name.data(), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file);
	if (handle != INVALID_HANDLE_VALUE) {
		opened = TRUE;
	} else {
		last_error.Update();
	}
	return opened;
}

inline BOOL MyLib::WinFile::WinCreateFile(DWORD desired_access, DWORD share_mode, DWORD creation_disposition, DWORD flags_and_attributes)
{
	return WinCreateFile(desired_access, share_mode, NULL, creation_disposition, flags_and_attributes, NULL);
}

inline BOOL MyLib::WinFile::WinCreateFile(DWORD creation_disposition, DWORD desired_access, DWORD flags_and_attributes)
{
	return WinCreateFile(desired_access, WINFILE_DEFAULT_VALUE__SHARE_MODE, NULL, creation_disposition, flags_and_attributes, NULL);
}

inline BOOL MyLib::WinFile::WinCreateFile(DWORD creation_disposition, DWORD desired_access)
{
	return WinCreateFile(desired_access, WINFILE_DEFAULT_VALUE__SHARE_MODE, NULL, creation_disposition, WINFILE_DEFAULT_VALUE__FLAGS_AND_ATTRIBUTES, NULL);
}

BOOL MyLib::WinFile::WinSetFilePointerEx(LARGE_INTEGER liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod)
{
	if (::SetFilePointerEx(handle, liDistanceToMove, lpNewFilePointer, dwMoveMethod)) {
		return TRUE;
	} else {
		last_error.Update();
		return FALSE;
	}
}

inline BOOL MyLib::WinFile::Open(DWORD file_mode)
{
	return WinCreateFile(OPEN_EXISTING, DesiredAccess(file_mode));
}

inline BOOL MyLib::WinFile::Open(DWORD file_mode, DWORD flags_and_attributes)
{
	return WinCreateFile(OPEN_EXISTING, DesiredAccess(file_mode), flags_and_attributes);;
}

inline BOOL MyLib::WinFile::Create(DWORD file_mode)
{
	return WinCreateFile(CREATE_ALWAYS, DesiredAccess(file_mode));
}

inline BOOL MyLib::WinFile::Create(DWORD file_mode, DWORD flags_and_attributes)
{
	return WinCreateFile(CREATE_ALWAYS, DesiredAccess(file_mode), flags_and_attributes);
}

inline void MyLib::WinFile::Close(void)
{
	if (handle != INVALID_HANDLE_VALUE) {
		::CloseHandle(handle);
	}
	name.clear();
	opened = FALSE;
	handle = INVALID_HANDLE_VALUE;
}

LONGLONG MyLib::WinFile::Pointer(void)
{
	LARGE_INTEGER zero_distance;
	LARGE_INTEGER current_pointer;
	zero_distance.QuadPart = 0;
	current_pointer.QuadPart = 0;
	if (WinSetFilePointerEx(zero_distance, &current_pointer, FILE_CURRENT)) {
		return current_pointer.QuadPart;
	} else {
		last_error.Update();
		return INVALID_FILE_POINTER;
	}
}

BOOL MyLib::WinFile::SetPointer(const LONGLONG &new_pointer)
{
	LARGE_INTEGER distance;
	distance.QuadPart = 0;
	if (WinSetFilePointerEx(distance, NULL, FILE_BEGIN)) {
		return TRUE;
	} else {
		last_error.Update();
		return FALSE;
	}
}

BOOL MyLib::WinFile::Read(void *buffer, DWORD size, DWORD &readed)
{
	if (::ReadFile(handle, buffer, size, &readed, NULL)) {
		return TRUE;
	} else {
		last_error.Update();
		return FALSE;
	}
}

BOOL MyLib::WinFile::Write(void *buffer, DWORD size, DWORD &written)
{
	if (::WriteFile(handle, buffer, size, &written, NULL)) {
		return TRUE;
	} else {
		last_error.Update();
		return FALSE;
	}
}

LONGLONG MyLib::WinFile::Size(void)
{
	LARGE_INTEGER size = {0};
	if (::GetFileSizeEx(handle, &size)) {
		return size.QuadPart;
	} else {
		last_error.Update();
		return INVALID_FILE_SIZE;
	}
}

BOOL MyLib::WinFile::SetFileSize(const LONGLONG &new_size)
{
	if (SetPointer(new_size)) {
		if (::SetEndOfFile(handle)) {
			return TRUE;
		}
	}
	last_error.Update();
	return FALSE;
}
