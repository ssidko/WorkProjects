#pragma once

#ifndef _TIMESTAMP
#define _TIMESTAMP

#include <windows.h>

class Timestamp
{
private:
	DWORD sec;
	DWORD mins;
	DWORD hours;
	DWORD day;
	DWORD mounth;
	DWORD year;
	LONGLONG seconds;
	char str[32];
public:
	Timestamp();
	Timestamp(DWORD year_, DWORD month_, DWORD day_, DWORD hours_, DWORD mins_, DWORD sec_);
	~Timestamp();

	void Clear(void);
	LONGLONG Seconds(void) const;
	const char *String(void);

	bool operator>(const Timestamp &t);
	bool operator<(const Timestamp &t);
	bool operator==(const Timestamp &t);
	LONGLONG operator-(const Timestamp &t);
};

#endif

