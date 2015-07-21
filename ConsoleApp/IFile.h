#ifndef _IFILE_H
#define _IFILE_H

#include <windows.h>
#include <tchar.h>

#include <string>
#include <assert.h>

class IFile
{
public:

	typedef enum FileMode {
		kReadOnly			= 1 << 1,
		kWriteOnly			= 1 << 2,
		kReadWrite			= kReadOnly | kWriteOnly,
	};

	virtual ~IFile(void) {}

	virtual BOOL IsOpen(void) = 0;
	virtual BOOL Open(DWORD file_mode) = 0;
	virtual BOOL Create(DWORD file_mode) = 0;
	virtual void Close(void) = 0;

	virtual LONGLONG Pointer(void) = 0;
	virtual BOOL SetPointer(LONGLONG new_pointer) = 0;

	virtual BOOL Read(void *buffer, DWORD size, DWORD &readed) = 0;
	virtual BOOL Write(void *buffer, DWORD size, DWORD &written) = 0;

	virtual LONGLONG Size(void);
};

class WinFile : public IFile
{
private:
	BOOL opened;
	HANDLE handle;
	std::basic_string<TCHAR> name;

public:
	WinFile(const TCHAR *file_name, DWORD file_mode) : opened(FALSE), handle(INVALID_HANDLE_VALUE), name(file_name) {}
	~WinFile(void) { Close(); }


	//
	// IFile interface.
	//

	virtual BOOL IsOpen(void) { return opened; }

	virtual BOOL Open(DWORD file_mode) 
	{


		return FALSE;
	}


	virtual BOOL Create(DWORD file_mode) { return FALSE; }
	virtual void Close(void) {}

	virtual LONGLONG Pointer(void) { return 0; }
	virtual BOOL SetPointer(LONGLONG new_pointer) { return FALSE; }

	virtual BOOL Read(void *buffer, DWORD size, DWORD &readed) { return FALSE; }
	virtual BOOL Write(void *buffer, DWORD size, DWORD &written) { return FALSE; }

	virtual LONGLONG Size(void) { return 0; }


	//
	// Extended functionality.
	//

	BOOL WinCreateFile(DWORD desired_access, DWORD share_mode, LPSECURITY_ATTRIBUTES security_attributes, DWORD creation_disposition, DWORD flags_and_attributes, HANDLE template_file)
	{
		opened = FALSE;
		handle = ::CreateFile(name.data(), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file);
		if (handle != INVALID_HANDLE_VALUE) {
			opened = TRUE;
		}
		return opened;
	}
};

#endif // _IFILE_H