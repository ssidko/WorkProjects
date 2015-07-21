#ifndef _ERROR_H
#define _ERROR_H

#include "MyLib.h"

MY_LIB_NAMESPACE_START

class Error
{
protected:
	DWORD code;
	std::basic_string<TCHAR> description;
public:
	Error(void);
	Error(Error &error);
	Error(DWORD error_code, const TCHAR *error_description);
	~Error(void);
	void Update(DWORD error_code, const TCHAR *error_description);
	virtual DWORD Code(void);
	virtual const TCHAR *Description(void);
};

class WinError : public Error
{
public:
	WinError(void);
	void Update(void);
};

MY_LIB_NAMESPACE_END

#endif // _ERROR_H