#include "WinConsole.h"

WinConsole::WinConsole(void) : output(NULL), colour(kWhite)
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
	CONSOLE_SCREEN_BUFFER_INFO buffer_info;
	memset(&buffer_info, 0x00, sizeof(CONSOLE_SCREEN_BUFFER_INFO));
	::GetConsoleScreenBufferInfo(output, &buffer_info);	
}

void WinConsole::RestoreColour(void)
{
	::SetConsoleTextAttribute(output, colour);
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
	Print(_T("####"), kWhite|kIntensity); Print(_T("####"), kWhite); Print(_T("####\n"), kIntensity);
	Print(_T("WinConsole ver.:")); Print(_T(" 2")); Print(_T(" 2"), kIntensity); Print(_T(" 0.1\n"), kWhite|kIntensity);
	Print(_T("New line\n"), kBlack, kIntensity);
	Print(_T("New line\n"), kBlue);
	Print(_T("New line\n"), kRed);
	Print(_T("Initial colour")); Print(_T(" 1"), kRed); Print(_T(" 2")); Print(_T(" 2"), kIntensity); Print(_T(" 3\n"), kBlue|kGreen);
}
