#include "Timestamp.h"
#include <stdio.h>

Timestamp::Timestamp()
{
	seconds = 0;
	minutes = 0;
	hours = 0;
	day = 0;
	month = 0;
	year = 0;
	memset(str, 0x00, sizeof(str));
}

Timestamp::Timestamp(DWORD year_, DWORD month_, DWORD day_, DWORD hours_, DWORD mins_, DWORD sec_)
{
	seconds = sec_;
	minutes = mins_;
	hours = hours_;
	day = day_;
	month = month_;
	year = year_;
	memset(str, 0x00, sizeof(str));
}

Timestamp::~Timestamp(void)
{
}

void Timestamp::Clear( void )
{
	memset(this, 0x00, sizeof(*this));
}

ULONGLONG Timestamp::Seconds( void ) const
{
	return (LONGLONG)((((year*12LL + month)*31LL + day)*24LL + hours)*60LL + minutes)*60LL + seconds;
}

const char * Timestamp::String( void )
{
	memset(str, 0x00, sizeof(str));
	sprintf_s(str, sizeof(str), "%04d-%02d-%02d--%02d-%02d-%02d",year,month,day,hours,minutes,seconds);
	return str;
}

bool Timestamp::operator>( const Timestamp &t )
{
	return (this->Seconds() > t.Seconds());
}

bool Timestamp::operator>=( const Timestamp &t )
{
	return (this->Seconds() >= t.Seconds());
}

bool Timestamp::operator<( const Timestamp &t )
{
	return (this->Seconds() < t.Seconds());
}

bool Timestamp::operator<=( const Timestamp &t )
{
	return (this->Seconds() <= t.Seconds());
}

bool Timestamp::operator==( const Timestamp &t )
{
	return (this->Seconds() == t.Seconds());
}

ULONGLONG Timestamp::operator-( const Timestamp &t )
{
	return (this->Seconds() - t.Seconds());
}
