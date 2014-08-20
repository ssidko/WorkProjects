#ifndef BITMAP_H
#define	BITMAP_H

#include <windows.h>

class BitMapLE
{
private:
	char *buffer;
	DWORD size;	
public:
	explicit BitMapLE(char *bit_buffer, DWORD bits_count);
	~BitMapLE(void);
	bool TestBit(const DWORD &zero_based_bit_index);
	void SetBit(const DWORD &zero_based_bit_index);
	void ClearBit(const DWORD &zero_based_bit_index);
	bool operator[](const DWORD &zero_based_bit_index);
	DWORD Size(void);
	DWORD OneCount(void); // количество единиц
	DWORD ZeroCount(void); // количество нулей
};

class BitMapBE
{
private:
	char *buffer;
	DWORD size;	
public:
	explicit BitMapBE(char *bit_buffer, DWORD bits_count);
	~BitMapBE(void);
	bool TestBit(const DWORD &zero_based_bit_index);
	void SetBit(const DWORD &zero_based_bit_index);
	void ClearBit(const DWORD &zero_based_bit_index);
	bool operator[](const DWORD &zero_based_bit_index);
	DWORD Size(void);
	DWORD OneCount(void); // количество единиц
	DWORD ZeroCount(void); // количество нулей
};

#endif

