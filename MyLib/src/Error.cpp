#include "stdafx.h"
#include "Error.h"

inline MyLib::Error::Error(void) : code(0), description(_T(""))
{
}

inline MyLib::Error::Error(Error &error)
{
	code = error.code;
	description = error.description;
}

inline MyLib::Error::Error(DWORD error_code, const TCHAR * error_description) : code(error_code), description(error_description)
{
}

inline MyLib::Error::~Error(void)
{
}

inline void MyLib::Error::Update(DWORD error_code, const TCHAR * error_description)
{
	code = error_code;
	description = error_description;
}

inline DWORD MyLib::Error::Code(void)
{
	return code;
}

inline const TCHAR *MyLib::Error::Description(void) 
{
	return description.data();
}

MyLib::WinError::WinError(void)
{
	Update();
}

void MyLib::WinError::Update(void)
{
	code = ::GetLastError();
	description.clear();
	if (code) {
		TCHAR *str = NULL;
		if (::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, code, 0, (LPTSTR)&str, 0, NULL)) {
			description = str;
			::LocalFree(str);
		}
	} 
}
