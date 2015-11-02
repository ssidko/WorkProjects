#ifndef _LOG_FILE_H
#define _LOG_FILE_H

#include <windows.h>
#include <string>

class LogFile
{
private:
	std::string file_name;
public:
	LogFile();
	~LogFile();

	void SetOutFile(const char *out_file_name);
	void PrintTimestamp(void);
	void PrintLine(const char *format, ...);
	LogFile &operator << (const char *text);
	LogFile &operator << (const DWORD value);
};

#endif // _LOG_FILE_H