#pragma once

#ifndef _TIMESTAMP_H
#define _TIMESTAMP_H

#include <windows.h>
#include <string>
#include <vector>

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

		Timestamp(WORD year_ = 0, BYTE month_ = 0, BYTE day_ = 0, BYTE hours_ = 0, BYTE mins_ = 0, BYTE sec_ = 0);
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

	struct Frame {
		LONGLONG offset;
		DWORD camera;
		Timestamp time;
		std::vector<BYTE> buffer;
	};

	struct FrameSequence {
		LONGLONG offset;
		Timestamp start_time;
		Timestamp end_time;
		std::vector<BYTE> buffer;
	};
}

#endif