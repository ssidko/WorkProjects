#ifndef _ERROR_H
#define _ERROR_H

#include "MyLib.h"

MY_LIB_NAMESPACE_START

class Error
{
private:
	DWORD code;
	std::basic_string<TCHAR> description;

public:
	Error(void) : code(0), description(_T("")) {}
	Error(Error &error);
	Error(DWORD error_code, const TCHAR *error_description) : code(error_code), description(error_description) {}

	~Error(void) {}

	virtual DWORD Code(void) { return code; }
	virtual const TCHAR *Description(void) { return description.data(); }
};

MY_LIB_NAMESPACE_END

#endif // _ERROR_H