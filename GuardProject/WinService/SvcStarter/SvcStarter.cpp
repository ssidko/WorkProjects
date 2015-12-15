#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>
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
	} else {
		std::cout << "Service installed.\n";
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

bool CurrentDirectory(std::wstring &current_directory)
{
	DWORD ret = ::GetCurrentDirectoryW(0, NULL);
	if (ret) {
		current_directory.resize((ret + 1)*sizeof(wchar_t));
		if (::GetCurrentDirectoryW(current_directory.size(), &current_directory[0])) {
			return true;
		}	
	}
	return false;	
}

int _tmain(int argc, _TCHAR* argv[])
{
	bool result = false;

	std::wstring service_module_path;
	std::wstring current_directory;
	if (CurrentDirectory(current_directory)) {
		service_module_path += current_directory;
		service_module_path += L"\\";
		service_module_path += L"GuardSystemSvc.exe";
		if (argc == 1) {
			result = InstallService(service_module_path.c_str());
		} else if (argc >= 2) {
			std::wstring argument = argv[1];
			if (argument == _T("install")) {
				InstallService(service_module_path.c_str());
			} else if (argument == _T("uninstall")) {
				std::wstring svc_name = argv[1];
				DeleteService(SERVICE_NAME);
			}
			else {
				std::cout << "Wrong 1st parameter. Must be [install] or [uninstall]\n";
			}
		}
	}

	::system("pause");

	return 0;
}

