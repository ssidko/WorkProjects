#include "main.h"
#include "MainWnd.h"
#include "Objidl.h"
#include "FlashRecovery.h"
#include "ext2fs.h"

using namespace FR;

LPDIRECT3D9 pD3D = NULL;
LPDIRECT3DDEVICE9 pd3dDevice = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	try
	{
		::AllocConsole();
		::SetConsoleCP(1251);

		printf("Test\n");

		TCHAR *str = _T("Да здавствует великий и могучий... )))");
		DWORD rw = 0;

		WriteConsole(::GetStdHandle(STD_OUTPUT_HANDLE), str, _tcslen(str), &rw, NULL);

		DWORD id = ::GetCurrentThreadId();

		W32App app;
		MainWnd wnd;
		wnd.Create(_T("Main Window"));
		wnd.CenterWindow();
		wnd.Show();
		return app.Run();
	}
	catch (Exception &e)
	{
		e.ShowMessage();
	}

	return 0;
}

