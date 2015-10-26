#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include "SvcGeneral.h"

bool InstallService(const TCHAR *svc_name)
{
	DWORD last_error = 0;
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
		return false;
	}

	std::cout << "Connected to SCM \n";

	SC_HANDLE svc_handle = CreateService(scm_handle,
		SERVICE_NAME, SERVICE_DISPLAY_NAME,
		GENERIC_READ | GENERIC_WRITE,
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
		SERVICE_AUTO_START,
		SERVICE_ERROR_NORMAL,
		svc_name,
		NULL, NULL, NULL, NULL, NULL);
	if (svc_handle == NULL) {
		last_error = ::GetLastError();
		std::cout << "Error code: " << last_error << "\n";
		return false;
	}

	::CloseServiceHandle(svc_handle);
	::CloseServiceHandle(scm_handle);

	return true;
}

bool DeleteService(const TCHAR *svc_name)
{
	DWORD last_error = 0;
	SC_HANDLE scm_handle = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
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
		return false;
	}

	SC_HANDLE svc_handle = OpenService( scm_handle, svc_name, DELETE);
	if (svc_handle == NULL){
		std::cout << "OpenService failed: " << ::GetLastError() << "\n";
		CloseServiceHandle(scm_handle);
		return false;
	}

	if (!DeleteService(svc_handle)) {
		std::cout << "DeleteService failed: " << GetLastError() << "\n";
	} else {
		std::cout << "Service deleted successfully\n";
	} 

	::CloseServiceHandle(svc_handle);
	::CloseServiceHandle(scm_handle);

	return true;
}

int _tmain(int argc, _TCHAR* argv[])
{
	bool result = false;

	//result = DeleteService(SERVICE_NAME);
	result = InstallService(_T("WinService.exe"));

	return 0;
}

