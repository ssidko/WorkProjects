#include "WinConsole.h"

#define INVALID_COLOUR			(DWORD)0xFFFFFFFF

WinConsole::WinConsole(void) : handle(NULL), saved_colour(INVALID_COLOUR) 
{
	::AllocConsole();
	::SetConsoleCP(DEFAULT_CP);
	handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
}

WinConsole::~WinConsole(void)
{
	::FreeConsole();
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
	} else {
		SetColour(kWhite, kBlack);
	}
}

void WinConsole::Print(const TCHAR *str)
{
	DWORD wr = 0;
	::WriteConsole(handle, str, _tcsclen(str), &wr, NULL);
}

void WinConsole::Print(const TCHAR *str, DWORD text_colour)
{
	SaveColour();
	SetColour(text_colour);
	Print(str);
	RestoreColour();
}

void WinConsole::Print(const TCHAR *str, DWORD text_colour, DWORD background_colour)
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

void WinConsole::Print(DWORD x, DWORD y, const TCHAR *str, DWORD text_colour)
{
	SavePosition();
	SetPosition(x, y);
	Print(str, text_colour);
	RestorePosition();
}

void WinConsole::Print(DWORD x, DWORD y, const TCHAR *str, DWORD text_colour, DWORD background_colour)
{
	SavePosition();
	SetPosition(x, y);
	Print(str, text_colour, background_colour);
	RestorePosition();
}

void WinConsole::SetColour(DWORD text_colour)
{
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	memset(&buffer_info, 0x00, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
	::GetConsoleScreenBufferInfo(handle, &buffer_info);
	::SetConsoleTextAttribute(handle, text_colour|((0x00F0&buffer_info.wAttributes)>>4));
}

void WinConsole::SetColour(DWORD text_colour, DWORD background_colour)
{
	::SetConsoleTextAttribute(handle, text_colour|(background_colour << 4));
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

void WinConsole::Test()
{
	Print(_T("WinConsole\n"));

	Print(_T("\n“ест основных цветов:\n"));

	Print(_T(" ####")); Print(_T(" - цвет по умолчанию\n"));
	Print(_T(" ####"), kWhite|kIntensity); Print(_T(" - интенсивный белый\n"));
	Print(_T(" ####"), kBlue); Print(_T(" - синий\n"));
	Print(_T(" ####"), kBlue|kIntensity); Print(_T(" - интенсивный синий\n"));
	Print(_T(" ####"), kGreen); Print(_T(" - зеленый\n"));
	Print(_T(" ####"), kGreen|kIntensity); Print(_T(" - интенсивный зеленый\n"));
	Print(_T(" ####"), kRed); Print(_T(" - красный\n"));
	Print(_T(" ####"), kRed|kIntensity); Print(_T(" - интенсивный красный\n"));	
	
	Print(_T("\n“ест смешаных цветов:\n"));
	Print(_T(" ####"), kBlue|kGreen); Print(_T(" - синий+зеленый\n"));
	Print(_T(" ####"), kBlue|kGreen|kIntensity); Print(_T(" - интенсивный синий+зеленый\n"));
	Print(_T(" ####"), kBlue|kRed); Print(_T(" - синий+красный\n"));
	Print(_T(" ####"), kBlue|kRed|kIntensity); Print(_T(" - интенсивный синий+красный\n"));
	Print(_T(" ####"), kGreen|kRed); Print(_T(" - зеленый+красный\n"));
	Print(_T(" ####"), kGreen|kRed|kIntensity); Print(_T(" - интенсивный зеленый+красный\n"));
}
