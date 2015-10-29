#include "stdafx.h"
#include "LogFile.h"

#include <fstream>
#include <time.h>

LogFile::LogFile()
{
}

LogFile::~LogFile()
{
}

void LogFile::SetOutFile(const char *out_file_name)
{
	file_name = out_file_name;
}

void LogFile::PrintTimestamp(void)
{
	tm time;
	__time64_t raw_time;
	char time_string[32] = { 0 };

	_time64(&raw_time);
	if (_localtime64_s(&time, &raw_time)) {
		return;
	}
	int res = sprintf_s(time_string, sizeof(time_string), "%04d/%02d/%02d %02d:%02d:%02d   ",
						time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec);
	if (res > 0) {
		this->operator<<(time_string);
	}
}

void LogFile::PrintLine(const char *format, ...)
{
	int len = 0;
	char *string = nullptr;
	va_list argptr = nullptr;

	va_start(argptr, format);
	len = (_vscprintf(format, argptr) + 2);
	if (len > 0) {
		string = new char[len];
		if (string) {
			memset(string, 0x00, len);
			if (vsprintf_s(string, len, format, argptr) > 0) {
				PrintTimestamp();
				this->operator<<(string);
				this->operator<<("\n");
			}
			delete[] string;
		}
	}
	va_end(argptr);
}

LogFile &LogFile::operator << (const char *text)
{
	std::fstream file(file_name.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
	if (file.is_open()) {
		file << text;
	}
	file.close();
	return *this;
}

LogFile &LogFile::operator << (const DWORD value)
{
	std::fstream file(file_name.c_str(), std::fstream::in | std::fstream::out | std::fstream::app);
	if (file.is_open()) {
		file << std::to_string(value).c_str();
	}
	file.close();
	return *this;
}
