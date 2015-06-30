#ifndef _WINCONSOLE_H
#define _WINCONSOLE_H

#include <windows.h>
#include <tchar.h>

#include <assert.h>

#define DEFAULT_CP				(1251)
#define NEW_LINE				(_T("\n"))

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
	HANDLE handle;
	DWORD saved_colour;
	COORD saved_position;
	TCHAR *str_buffer;

	void SaveColour(void);
	void RestoreColour(void);

	void SavePosition(void);
	void RestorePosition(void);
public:
	WinConsole(void);
	~WinConsole(void);

	void Print(const TCHAR *str);
	void Print(const TCHAR *str, DWORD text_colour);
	void Print(const TCHAR *str, DWORD text_colour, DWORD background_colour);

	void Print(DWORD x, DWORD y, const TCHAR *str);
	void Print(DWORD x, DWORD y, const TCHAR *str, DWORD text_colour);
	void Print(DWORD x, DWORD y, const TCHAR *str, DWORD text_colour, DWORD background_colour);

	void Printf(const TCHAR *format, ...);
	void Printf(DWORD text_colour, const TCHAR *format, ...);

	void SetTextColour(DWORD text_colour);
	void SetBackgroundColour(DWORD background_colour);
	void SetColour(DWORD text_colour, DWORD background_colour);

	void SetPosition(DWORD x, DWORD y);

	WinConsole & operator<<(TCHAR *str);
	WinConsole & operator<<(int value);
	WinConsole & operator<<(LONGLONG value);

	void Test();
};


#endif

