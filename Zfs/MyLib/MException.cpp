#include "MException.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            MException
////////////////////////////////////////////////////////////////////////////////////////////////////////

MException::MException()
{
	error_message = "Unknown exception.";
}

MException::MException(const std::string &_error_message) : error_message(_error_message)
{
}

MException::~MException()
{
}

const char *MException::what(void)
{
	return error_message.c_str();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                            WinError
////////////////////////////////////////////////////////////////////////////////////////////////////////

WinError::WinError() : error_code(0)
{
	error_code = ::GetLastError();
	if (error_code) {
		FormatErrorString(error_code, error_message);
	}
}

WinError::~WinError()
{

}

void WinError::FormatErrorString(DWORD error, std::string &error_string)
{
	CHAR *str = NULL;
	error_string.clear();
	if (::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, (LPSTR)&str, 0, NULL)) {
		error_string = str;
		::LocalFree(str);
	}
}
