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
		WORD year = 0;
		BYTE month = 0;
		BYTE day = 0;
		BYTE hours = 0;
		BYTE minutes = 0;
		BYTE seconds = 0;

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
		operator bool(void);
	};

	struct Frame {
		LONGLONG offset = 0;
		DWORD camera = 0;
		dvr::Timestamp time;
		std::vector<uint8_t> buffer;

		void Clear(void);
	};

	struct FrameSequence {
		LONGLONG offset = 0;
		DWORD camera = 0;
		size_t frames_count = 0;
		dvr::Timestamp start_time;
		dvr::Timestamp end_time;
		std::vector<uint8_t> buffer;
		
		void Clear(void);
	};
}

#endif