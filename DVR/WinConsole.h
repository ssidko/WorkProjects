#ifndef _WINCONSOLE_H
#define _WINCONSOLE_H

#include <windows.h>
#include <tchar.h>

#define DEFAULT_CP		(1251)

enum ConsoleColour {
	kBlack		= 0x0000,
	kBlue		= 0x0001,
	kGreen		= 0x0002,
	kRed		= 0x0004,
	kWhite		= 0x0007, 
	kIntensity	= 0x0008,
};

class WinConsole
{
private:
	HANDLE output;
	WORD colour;
	void SaveColour(void);
	void RestoreColour(void);
public:
	WinConsole(void);
	~WinConsole(void);
	void Print(const TCHAR *str);
	void Print(const TCHAR *str, WORD text_colour);
	void Print(const TCHAR *str, WORD text_colour, WORD background_colour);
	void SetTextColour(WORD text_colour);
	void SetTextColour(WORD text_colour, WORD background_colour);
	void SetPosition(short x, short y);
	void Test();
};

#endif

