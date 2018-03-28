#include "dvr.h"
#include <stdio.h>
#include <cctype>

dvr::Timestamp::Timestamp(WORD year_, BYTE month_, BYTE day_, BYTE hours_, BYTE mins_, BYTE sec_) :
	year(year_),
	month(month_),
	day(day_),
	hours(hours_),
	minutes(mins_),
	seconds(sec_)
{
}

void dvr::Timestamp::Clear( void )
{
	year = month = day = hours = minutes = seconds = 0;
}

ULONGLONG dvr::Timestamp::Seconds( void ) const
{
	return (ULONGLONG)((((year*12LL + month)*31LL + day)*24LL + hours)*60LL + minutes)*60LL + seconds;
}

std::string dvr::Timestamp::ToString(void)
{
	char str[32];
	memset(str, 0x00, sizeof(str));
	sprintf_s(str, sizeof(str), "%04u-%02u-%02u--%02u-%02u-%02u", year, month, day, hours, minutes, seconds);
	return std::string(str);
}

void dvr::Timestamp::ToString(std::string & str)
{
	str = ToString();
}

std::string dvr::Timestamp::Date(char separator)
{
	char str[12];
	memset(str, 0x00, sizeof(str));
	separator = std::isprint(separator) ? separator : ' ';
	sprintf_s(str, sizeof(str), "%04u%c%02u%c%02u", year, separator, month, separator, day);
	return std::string(str);
}

std::string dvr::Timestamp::Time(char separator)
{
	char str[12];
	memset(str, 0x00, sizeof(str));
	separator = std::isprint(separator) ? separator : ' ';
	sprintf_s(str, sizeof(str), "%02u%c%02u%c%02u", hours, separator, minutes, separator, seconds);
	return std::string(str);
}

bool dvr::Timestamp::operator>( const Timestamp &t )
{
	return (this->Seconds() > t.Seconds());
}

bool dvr::Timestamp::operator>=( const Timestamp &t )
{
	return (this->Seconds() >= t.Seconds());
}

bool dvr::Timestamp::operator<( const Timestamp &t )
{
	return (this->Seconds() < t.Seconds());
}

bool dvr::Timestamp::operator<=( const Timestamp &t )
{
	return (this->Seconds() <= t.Seconds());
}

bool dvr::Timestamp::operator==( const Timestamp &t )
{
	return (this->Seconds() == t.Seconds());
}

ULONGLONG dvr::Timestamp::operator-( const Timestamp &t )
{
	return (this->Seconds() - t.Seconds());
}

dvr::Timestamp::operator bool(void)
{
	return year ? true : false;
}

void dvr::Frame::Clear(void)
{
	offset = 0;
	camera = 0;
	time.Clear();
	buffer.clear();
}

void dvr::FrameSequence::Clear(void)
{
	first_frame_offset = 0;
	last_frame_offset = 0;
	camera = 0;
	frames_count = 0;
	start_time.Clear();
	end_time.Clear();
	buffer.clear();
}

bool dvr::VideoStorage::SaveFrameSequence(FrameSequence &seq)
{

	
}

