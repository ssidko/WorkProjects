#pragma once

#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <windows.h>
#include <string>

namespace dvr
{
	class Timestamp
	{
	public:
		WORD year;
		BYTE month;
		BYTE day;
		BYTE hours;
		BYTE minutes;
		BYTE seconds;

		Timestamp();
		Timestamp(WORD year_, BYTE month_, BYTE day_, BYTE hours_, BYTE mins_, BYTE sec_);
		void Clear(void);
		ULONGLONG Seconds(void) const;
		std::string ToString(void);
		void ToString(std::string &str);
		std::string Date(char separator = '-');
		std::string Time(char separator = '-');

		bool operator>(const Timestamp &t);
		bool operator>=(const Timestamp &t);
		bool operator<(const Timestamp &t);
		bool operator<=(const Timestamp &t);
		bool operator==(const Timestamp &t);
		ULONGLONG operator-(const Timestamp &t);
	};
}


#endif

