#include "stdafx.h"
#include <stdlib.h>
#include "WinConsole.h"

#define INVALID_COLOUR			(WORD)0xFFFF
#define STR_BUFFER_SIZE			(DWORD)100

WinConsole::WinConsole(void) : handle(NULL), saved_colour(INVALID_COLOUR), str_buffer(nullptr)
{
	::AllocConsole();
	::SetConsoleCP(DEFAULT_CP);
	handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	str_buffer = new TCHAR[STR_BUFFER_SIZE];
	saved_position.X = 0;
	saved_position.Y = 0;
}

WinConsole::~WinConsole(void)
{
	::FreeConsole();
	if (str_buffer) {
		delete[] str_buffer;
	}
}

void WinConsole::SaveColour(void)
{
	CONSOLE_SCREEN_BUFFER_INFO screen_info;
	memset(&screen_info, 0x00, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
	::GetConsoleScreenBufferInfo(handle, &screen_info);
	saved_colour = screen_info.wAttributes;
}

void WinConsole::RestoreColour(void)
{
	if (saved_colour != INVALID_COLOUR) {
		::SetConsoleTextAttribute(handle, saved_colour);
	}
	else {
		SetColour(kWhite, kBlack);
	}
}

void WinConsole::Print(const TCHAR *str)
{
	DWORD wr = 0;
	assert(str);
	::WriteConsole(handle, str, _tcsclen(str), &wr, NULL);
}

void WinConsole::Print(const TCHAR *str, WORD text_colour)
{
	SaveColour();
	SetTextColour(text_colour);
	Print(str);
	RestoreColour();
}

void WinConsole::Print(const TCHAR *str, WORD text_colour, WORD background_colour)
{
	SaveColour();
	SetColour(text_colour, background_colour);
	Print(str);
	RestoreColour();
}

void WinConsole::Print(DWORD x, DWORD y, const TCHAR *str)
{
	SavePosition();
	SetPosition(x, y);
	Print(str);
	RestorePosition();
}

void WinConsole::Print(DWORD x, DWORD y, const TCHAR *str, WORD text_colour)
{
	SavePosition();
	SetPosition(x, y);
	Print(str, text_colour);
	RestorePosition();
}

void WinConsole::Print(DWORD x, DWORD y, const TCHAR *str, WORD text_colour, WORD background_colour)
{
	SavePosition();
	SetPosition(x, y);
	Print(str, text_colour, background_colour);
	RestorePosition();
}

void WinConsole::Printf(const TCHAR *format, ...)
{
	int len = 0;
	TCHAR *string = nullptr;
	va_list argptr = nullptr;

	va_start(argptr, format);
	len = (_vsctprintf(format, argptr) + 1);
	string = new TCHAR[len];
	_vstprintf_s(string, len, format, argptr);
	va_end(argptr);

	Print(string);

	delete[] string;
}

void WinConsole::Printf(WORD text_colour, const TCHAR *format, ...)
{
	int len = 0;
	TCHAR *string = nullptr;
	va_list argptr = nullptr;

	SaveColour();
	SetTextColour(text_colour);
	va_start(argptr, format);
	len = (_vsctprintf(format, argptr) + 1);
	string = new TCHAR[len];
	_vstprintf_s(string, len, format, argptr);
	va_end(argptr);
	Print(string);
	RestoreColour();

	delete[] string;

}

void WinConsole::SetTextColour(WORD text_colour)
{
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	memset(&buffer_info, 0x00, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
	::GetConsoleScreenBufferInfo(handle, &buffer_info);
	::SetConsoleTextAttribute(handle, text_colour | (buffer_info.wAttributes & 0xFFF0));
}

void WinConsole::SetBackgroundColour(WORD background_colour)
{
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	memset(&buffer_info, 0x00, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
	::GetConsoleScreenBufferInfo(handle, &buffer_info);
	::SetConsoleTextAttribute(handle, (background_colour << 4) | (buffer_info.wAttributes & 0xFF0F));
}

void WinConsole::SetColour(WORD text_colour, WORD background_colour)
{
	::SetConsoleTextAttribute(handle, text_colour | (background_colour << 4));
}

void WinConsole::SetPosition(DWORD x, DWORD y)
{
	COORD pos = { (short)x, (short)y };
	BOOL res = ::SetConsoleCursorPosition(handle, pos);
}

void WinConsole::SavePosition(void)
{
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	memset(&buffer_info, 0x00, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
	if (::GetConsoleScreenBufferInfo(handle, &buffer_info)) {
		saved_position = buffer_info.dwCursorPosition;
	}
}

void WinConsole::RestorePosition(void)
{
	::SetConsoleCursorPosition(handle, saved_position);
}

WinConsole &WinConsole::operator<<(TCHAR *str)
{
	Print(str);
	return *this;
}

WinConsole &WinConsole::operator<<(int value)
{
	_itot_s(value, str_buffer, STR_BUFFER_SIZE, 10);
	Print(str_buffer);
	return *this;
}

WinConsole &WinConsole::operator<<(LONGLONG value)
{
	_i64tot_s(value, str_buffer, STR_BUFFER_SIZE, 10);
	Print(str_buffer);
	return *this;
}

void WinConsole::Test()
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
