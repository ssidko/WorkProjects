#include "stdafx.h"
#include <windows.h>

void WINAPI ServiceMain(DWORD dwNumServicesArgs, LPWSTR  *lpServiceArgVectors)
{
	int x = 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	SERVICE_TABLE_ENTRY svc_entry;
	memset(&svc_entry, 0x00, sizeof(SERVICE_TABLE_ENTRY));
	svc_entry.lpServiceName = L"";
	svc_entry.lpServiceProc = ServiceMain;

	StartServiceCtrlDispatcher(&svc_entry);

	return 0;
}