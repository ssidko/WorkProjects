#ifndef _ERROR_H
#define _ERROR_H

#include "MyLib.h"

MY_LIB_NAMESPACE_START

class MyError
{
protected:
	DWORD code;
	std::basic_string<TCHAR> description;
public:
	MyError(void);
	MyError(MyError &error);
	MyError(DWORD error_code, const TCHAR *error_description);
	
	~MyError(void);

	void Update(DWORD error_code, const TCHAR *error_description);
	virtual DWORD Code(void);
	virtual const TCHAR *Description(void);
};

class WinError : public MyError
{
public:
	WinError(void);
	void Update(void);
};

MY_LIB_NAMESPACE_END

#endif // _ERROR_H