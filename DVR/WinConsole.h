#ifndef _WINCONSOLE_H
#define _WINCONSOLE_H

#include <windows.h>
#include <tchar.h>

#include <assert.h>

#define DEFAULT_CP				(1251)
#define INVALID_COLOUR			(DWORD)0xFFFFFFFF
#define STR_BUFFER_SIZE			(DWORD)100

enum ConsoleColour {
	kBlack		= 0x0000,
	kBlue		= 0x0001,
	kGreen		= 0x0002,
	kRed		= 0x0004,
	kWhite		= 0x0007, 
	kIntensity	= 0x0008,
};

template<typename CharType>
class WinConsole
{
private:
	HANDLE handle;
	DWORD saved_colour;
	COORD saved_position;
	CharType *str_buffer;
	void SaveColour(void);
	void RestoreColour(void);

	template<typename T> void PrintT(const T *str);

public:
	WinConsole(void);
	~WinConsole(void);

	void Print(const CharType *str);
	void Print(const CharType *str, DWORD text_colour);
	void Print(const CharType *str, DWORD text_colour, DWORD background_colour);

	void Print(DWORD x, DWORD y, const CharType *str);
	void Print(DWORD x, DWORD y, const CharType *str, DWORD text_colour);
	void Print(DWORD x, DWORD y, const CharType *str, DWORD text_colour, DWORD background_colour);

	void SetColour(DWORD text_colour);
	void SetColour(DWORD text_colour, DWORD background_colour);

	void SetPosition(DWORD x, DWORD y);
	void SavePosition(void);
	void RestorePosition(void);

	WinConsole & operator<<(CharType *str);
	WinConsole & operator<<(int value);
	WinConsole & operator<<(LONGLONG value);

	void Test();
};

template<typename CharType>
WinConsole<CharType>::WinConsole(void) : handle(NULL), saved_colour(INVALID_COLOUR), str_buffer(nullptr)
{
	::AllocConsole();
	::SetConsoleCP(DEFAULT_CP);
	handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	str_buffer = new CharType[STR_BUFFER_SIZE];
}

template<typename CharType>
WinConsole<CharType>::~WinConsole(void)
{
	::FreeConsole();
	if (str_buffer) {
		delete[] str_buffer;
	}
}

template<typename CharType>
void WinConsole<CharType>::SaveColour(void)
{
	CONSOLE_SCREEN_BUFFER_INFO screen_info;
	memset(&screen_info, 0x00, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
	::GetConsoleScreenBufferInfo(handle, &screen_info);
	saved_colour = screen_info.wAttributes;
}

template<typename CharType>
void WinConsole<CharType>::RestoreColour(void)
{
	if (saved_colour != INVALID_COLOUR) {
		::SetConsoleTextAttribute(handle, saved_colour);
	}
	else {
		SetColour(kWhite, kBlack);
	}
}

template<typename CharType>
template<typename T>
void WinConsole<CharType>::PrintT(const T *str)
{
	DWORD wr = 0;
	assert(str);
}

template<>
template<>
void WinConsole<char>::PrintT<char>(const char *str)
{
	DWORD wr = 0;
	assert(str);
	::WriteConsoleA(handle, str, strlen(str), &wr, NULL);
}

//template<typename CharType>
//template<>
//void WinConsole<CharType>::PrintT<wchar_t>(const wchar_t *str)
//{
//	DWORD wr = 0;
//	assert(str);
//	::WriteConsoleW(handle, str, wcslen(str), &wr, NULL);
//}
//
//template<typename CharType>
//template<>
//void WinConsole<CharType>::PrintT<TCHAR>(const TCHAR *str)
//{
//	DWORD wr = 0;
//	assert(str);
//	::WriteConsole(handle, str, _tcsclen(str), &wr, NULL);
//}

template<typename CharType>
void WinConsole<CharType>::Print(const CharType *str)
{
	PrintT<CharType>(str);
}

template<typename CharType>
void WinConsole<CharType>::Print(const CharType *str, DWORD text_colour)
{
	SaveColour();
	SetColour(text_colour);
	Print(str);
	RestoreColour();
}

template<typename CharType>
void WinConsole<CharType>::Print(const CharType *str, DWORD text_colour, DWORD background_colour)
{
	SaveColour();
	SetColour(text_colour, background_colour);
	Print(str);
	RestoreColour();
}

template<typename CharType>
void WinConsole<CharType>::Print(DWORD x, DWORD y, const CharType *str)
{
	SavePosition();
	SetPosition(x, y);
	Print(str);
	RestorePosition();
}

template<typename CharType>
void WinConsole<CharType>::Print(DWORD x, DWORD y, const CharType *str, DWORD text_colour)
{
	SavePosition();
	SetPosition(x, y);
	Print(str, text_colour);
	RestorePosition();
}

template<typename CharType>
void WinConsole<CharType>::Print(DWORD x, DWORD y, const CharType *str, DWORD text_colour, DWORD background_colour)
{
	SavePosition();
	SetPosition(x, y);
	Print(str, text_colour, background_colour);
	RestorePosition();
}

template<typename CharType>
void WinConsole<CharType>::SetColour(DWORD text_colour)
{
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	memset(&buffer_info, 0x00, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
	::GetConsoleScreenBufferInfo(handle, &buffer_info);
	::SetConsoleTextAttribute(handle, text_colour | ((0x00F0 & buffer_info.wAttributes) >> 4));
}

template<typename CharType>
void WinConsole<CharType>::SetColour(DWORD text_colour, DWORD background_colour)
{
	::SetConsoleTextAttribute(handle, text_colour | (background_colour << 4));
}

template<typename CharType>
void WinConsole<CharType>::SetPosition(DWORD x, DWORD y)
{
	COORD pos = { (short)x, (short)y };
	BOOL res = ::SetConsoleCursorPosition(handle, pos);
}

template<typename CharType>
void WinConsole<CharType>::SavePosition(void)
{
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	memset(&buffer_info, 0x00, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
	if (::GetConsoleScreenBufferInfo(handle, &buffer_info)) {
		saved_position = buffer_info.dwCursorPosition;
	}
}

template<typename CharType>
void WinConsole<CharType>::RestorePosition(void)
{
	::SetConsoleCursorPosition(handle, saved_position);
}

template<typename CharType>
WinConsole<CharType> & WinConsole<CharType>::operator<<(CharType *str)
{
	Print(str);
	return *this;
}

template<typename CharType>
WinConsole<CharType> & WinConsole<CharType>::operator<<(int value)
{
	_itot_s(value, str_buffer, STR_BUFFER_SIZE, 10);
	Print(str_buffer);
	return *this;
}

template<typename CharType>
WinConsole<CharType> & WinConsole<CharType>::operator<<(LONGLONG value)
{
	_i64tot_s(value, str_buffer, STR_BUFFER_SIZE, 10);
	Print(str_buffer);
	return *this;
}

template<typename CharType>
void WinConsole<CharType>::Test()
{
	Print(_T("WinConsole\n"));

	Print(_T("\n“ест основных цветов:\n"));

	Print(_T(" ####")); Print(_T(" - цвет по умолчанию\n"));
	Print(_T(" ####"), kWhite | kIntensity); Print(_T(" - интенсивный белый\n"));
	Print(_T(" ####"), kBlue); Print(_T(" - синий\n"));
	Print(_T(" ####"), kBlue | kIntensity); Print(_T(" - интенсивный синий\n"));
	Print(_T(" ####"), kGreen); Print(_T(" - зеленый\n"));
	Print(_T(" ####"), kGreen | kIntensity); Print(_T(" - интенсивный зеленый\n"));
	Print(_T(" ####"), kRed); Print(_T(" - красный\n"));
	Print(_T(" ####"), kRed | kIntensity); Print(_T(" - интенсивный красный\n"));

	Print(_T("\n“ест смешаных цветов:\n"));
	Print(_T(" ####"), kBlue | kGreen); Print(_T(" - синий+зеленый\n"));
	Print(_T(" ####"), kBlue | kGreen | kIntensity); Print(_T(" - интенсивный синий+зеленый\n"));
	Print(_T(" ####"), kBlue | kRed); Print(_T(" - синий+красный\n"));
	Print(_T(" ####"), kBlue | kRed | kIntensity); Print(_T(" - интенсивный синий+красный\n"));
	Print(_T(" ####"), kGreen | kRed); Print(_T(" - зеленый+красный\n"));
	Print(_T(" ####"), kGreen | kRed | kIntensity); Print(_T(" - интенсивный зеленый+красный\n"));
}

#endif

