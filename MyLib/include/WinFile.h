#ifndef _WIN_FILE_H
#define _WIN_FILE_H

#include "MyLib.h"

MY_LIB_NAMESPACE_START

class WinFile : public iFile
{
private:
	BOOL opened;
	HANDLE handle;
	std::basic_string<TCHAR> name;
	WinError last_error;

	DWORD DesiredAccess(DWORD file_mode);
	BOOL WinCreateFile(DWORD desired_access, DWORD share_mode, LPSECURITY_ATTRIBUTES security_attributes, DWORD creation_disposition, DWORD flags_and_attributes, HANDLE template_file);
	BOOL WinCreateFile(DWORD desired_access, DWORD share_mode, DWORD creation_disposition, DWORD flags_and_attributes);
	BOOL WinCreateFile(DWORD creation_disposition, DWORD desired_access, DWORD flags_and_attributes);
	BOOL WinCreateFile(DWORD creation_disposition, DWORD desired_access);
	BOOL WinSetFilePointerEx(LARGE_INTEGER  liDistanceToMove, PLARGE_INTEGER lpNewFilePointer, DWORD dwMoveMethod);
public:
	WinFile(const TCHAR *file_name, DWORD file_mode);
	~WinFile(void);

	 /*--== IFile interface ==--*/
	inline BOOL IsOpen(void) { return opened; }
	virtual BOOL Open(DWORD file_mode);
	virtual BOOL Create(DWORD file_mode);
	virtual void Close(void);
	virtual LONGLONG Pointer(void);
	virtual BOOL SetPointer(const LONGLONG &new_pointer);
	virtual BOOL Read(void *buffer, DWORD size, DWORD &readed);
	virtual BOOL Write(void *buffer, DWORD size, DWORD &written);
	virtual LONGLONG Size(void);

	/*--== Extended functionality ==--*/
	BOOL Open(DWORD file_mode, DWORD flags_and_attributes);
	BOOL Create(DWORD file_mode, DWORD flags_and_attributes);
	BOOL SetFileSize(const LONGLONG &new_size);
};

MY_LIB_NAMESPACE_END

#endif // _WIN_FILE_H