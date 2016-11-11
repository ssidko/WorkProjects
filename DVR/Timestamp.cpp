#include "Timestamp.h"
#include <stdio.h>


dvr::Timestamp::Timestamp()
{
	seconds = 0;
	minutes = 0;
	hours = 0;
	day = 0;
	month = 0;
	year = 0;
	memset(str, 0x00, sizeof(str));
}

dvr::Timestamp::Timestamp(DWORD year_, DWORD month_, DWORD day_, DWORD hours_, DWORD mins_, DWORD sec_)
{
	seconds = sec_;
	minutes = mins_;
	hours = hours_;
	day = day_;
	month = month_;
	year = year_;
	memset(str, 0x00, sizeof(str));
}

dvr::Timestamp::~Timestamp(void)
{
}

void dvr::Timestamp::Clear( void )
{
	memset(this, 0x00, sizeof(*this));
}

ULONGLONG dvr::Timestamp::Seconds( void ) const
{
	return (LONGLONG)((((year*12LL + month)*31LL + day)*24LL + hours)*60LL + minutes)*60LL + seconds;
}

const char * dvr::Timestamp::String( void )
{
	memset(str, 0x00, sizeof(str));
	sprintf_s(str, sizeof(str), "%04u-%02u-%02u--%02u-%02u-%02u",year,month,day,hours,minutes,seconds);
	return str;
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
