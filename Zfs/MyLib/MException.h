#ifndef _MEXCEPTION_H
#define _MEXCEPTION_H

#include <exception>
#include <string>

#include <windows.h>

class MException : public std::exception
{
private:
	std::string error_message;
public:
	MException();
	MException(const std::string &_error_message);
	~MException();
	virtual const char *what(void);
};

class WinError
{
private:
	DWORD error_code;
	std::string error_message;
public:
	WinError();
	~WinError();
	DWORD ErrorCode(void) { return error_code; }
	const std::string &ErrorString(void) { return error_message; }
	static void FormatErrorString(DWORD error, std::string &error_string);
};

#endif // _MEXCEPTION_H

