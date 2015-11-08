#pragma once

#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <windows.h>

typedef struct _TIMESTAMP {
	DWORD seconds;
	DWORD minutes;
	DWORD hours;
	DWORD day;
	DWORD month;
	DWORD year;
} TIMESTAMP;

class Timestamp : public TIMESTAMP
{
private:
	char str[32];
public:
	Timestamp();
	Timestamp(DWORD year_, DWORD month_, DWORD day_, DWORD hours_, DWORD mins_, DWORD sec_);
	~Timestamp();

	void Clear(void);

	void SetYear(DWORD y) {year = y;}
	void SetMonth(DWORD m) {month = m;}
	void SetDay(DWORD d) {day = d;}
	void SetHours(DWORD h) {hours = h;}
	void SetMinutes(DWORD m) {minutes = m;}
	void SetSeconds(DWORD s) {seconds = s;}

	ULONGLONG Seconds(void) const;
	const char *String(void);

	bool operator>(const Timestamp &t);
	bool operator>=(const Timestamp &t);
	bool operator<(const Timestamp &t);
	bool operator<=(const Timestamp &t);
	bool operator==(const Timestamp &t);
	ULONGLONG operator-(const Timestamp &t);
};

#endif

