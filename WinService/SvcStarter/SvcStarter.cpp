#include "stdafx.h"
#include <windows.h>
#include <iostream>

#define SERVICE_NAME			L"GuardSystem log"
#define SERVICE_DISPLAY_NAME	L"Loging service for GuardSystem"

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD last_error = 2000;

	SC_HANDLE scm_handle = OpenSCManager(NULL, NULL, GENERIC_READ | GENERIC_WRITE);
	if (scm_handle == NULL) {
		last_error = ::GetLastError();
		switch (last_error) {
		case ERROR_ACCESS_DENIED:
			std::cout << "Error: ERROR_ACCESS_DENIED \n";
			break;
		case ERROR_DATABASE_DOES_NOT_EXIST:
			std::cout << "Error: ERROR_DATABASE_DOES_NOT_EXIST \n";
			break;
		default:
			std::cout << "Error: Unknown error code " << last_error << "\n";
			break;
		}
		return -1;
	}

	std::cout << "Connected to SCM \n";

	SC_HANDLE svc_handle = CreateService(scm_handle,
		SERVICE_NAME, SERVICE_DISPLAY_NAME,
		GENERIC_READ | GENERIC_WRITE,
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		L"\"C:\\Users\\ssidko.EPOS_MAIN\\Documents\\Visual Studio 2013\\Projects\\WinService\\Debug\\SvcStarter.exe\"",
		NULL, NULL, NULL, NULL, NULL);
	if (svc_handle == NULL) {
		last_error = ::GetLastError();
		std::cout << "Error code: " << last_error << "\n";
		return -1;
	}

	::CloseServiceHandle(svc_handle);
	::CloseServiceHandle(scm_handle);

	return 0;
}

