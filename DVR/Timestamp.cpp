#include "Timestamp.h"
#include <stdio.h>

Timestamp::Timestamp() : sec(0),mins(0),hours(0),day(0),month(0),year(0)
{
}

Timestamp::Timestamp(DWORD year_, DWORD month_, DWORD day_, DWORD hours_, DWORD mins_, DWORD sec_)
{
	sec = sec_;
	mins = mins_;
	hours = hours_;
	day = day_;
	month = month_;
	year = year_;
}

Timestamp::~Timestamp(void)
{
}

void Timestamp::Clear( void )
{
	memset(this, 0x00, sizeof(*this));
}

LONGLONG Timestamp::Seconds( void ) const
{
	return (LONGLONG)(((year*365 + month*30 + day)*24 + hours)*60 + mins)*60 + sec;
}

const char * Timestamp::String( void )
{
	memset(str, 0x00, sizeof(str));
	sprintf_s(str, sizeof(str), "%04d-%02d-%02d %02d-%02d-%02d",year,month,day,hours,mins,sec);
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

LONGLONG Timestamp::operator-( const Timestamp &t )
{
	return (this->Seconds() - t.Seconds());
}
