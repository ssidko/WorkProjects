#include "WinConsole.h"

#define INVALID_COLOUR			(WORD)0xFFFF

WinConsole::WinConsole(void) : output(NULL), colour(INVALID_COLOUR)
{
	::AllocConsole();
	::SetConsoleCP(DEFAULT_CP);
	output = ::GetStdHandle(STD_OUTPUT_HANDLE);
}

WinConsole::~WinConsole(void)
{
	::FreeConsole();
}

void WinConsole::SaveColour(void)
{
	CONSOLE_SCREEN_BUFFER_INFO screen_info;
	memset(&screen_info, 0x00, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
	::GetConsoleScreenBufferInfo(output, &screen_info);
	colour = screen_info.wAttributes;
}

void WinConsole::RestoreColour(void)
{
	if (colour != INVALID_COLOUR) {
		::SetConsoleTextAttribute(output, colour);
	} else {
		SetTextColour(kWhite, kBlack);
	}
}

void WinConsole::Print(const TCHAR *str)
{
	DWORD wr = 0;
	::WriteConsole(output, str, _tcsclen(str), &wr, NULL);
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
	SetTextColour(text_colour, background_colour);
	Print(str);
	RestoreColour();
}

void WinConsole::SetTextColour(WORD text_colour)
{
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	memset(&buffer_info, 0x00, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
	::GetConsoleScreenBufferInfo(output, &buffer_info);
	::SetConsoleTextAttribute(output, text_colour|((0x00F0&buffer_info.wAttributes)>>4));
}

void WinConsole::SetTextColour(WORD text_colour, WORD background_colour)
{
	::SetConsoleTextAttribute(output, text_colour|(background_colour << 4));
}

void WinConsole::SetPosition(short x, short y)
{
	COORD pos = {x, y};
	BOOL res = ::SetConsoleCursorPosition(output, pos);
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
