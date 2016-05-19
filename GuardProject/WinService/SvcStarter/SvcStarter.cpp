#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include "SvcGeneral.h"

bool FileExists(const std::wstring &file_path)
{
	DWORD dwAttrib = GetFileAttributes(file_path.data());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

DWORD ServiceState(SC_HANDLE svc_handle)
{
	SERVICE_STATUS svc_status;
	memset(&svc_status, 0x00, sizeof(SERVICE_STATUS));

	if (::QueryServiceStatus(svc_handle, &svc_status)) {
		return svc_status.dwCurrentState;	
	}
	else {
		DWORD last_error = ::GetLastError();
		std::cout << "Error in ::QueryServiceStatus(). Error code: " << last_error << ".\n";
	}
	return 0;
}

bool TryRunService(SC_HANDLE svc_handle)
{
	DWORD last_error = 0;
	bool result = false;

	std::cout << "Starting service ";

	if (StartService(svc_handle, 0, NULL)) {
	
		DWORD state = 0;
		DWORD start = ::GetTickCount();

		while (state = ServiceState(svc_handle)) {
			std::cout << ".";
			if (state == SERVICE_RUNNING) {
				std::cout << "\nService started.\n";
				result = true;
				break;
			}
			if ((::GetTickCount() - start) > 30 * 1000) {
				std::cout << "\nError: service start timeout reached.\n";
				result = false;
				break;
			}
			::Sleep(500);		
		}	
	} 

	if (!result) {
		last_error = ::GetLastError();
		std::cout << "\nError in TryRunService(). Error code: " << last_error << ".\n";
	}
	return result;
}

bool TryStopService(SC_HANDLE svc_handle)
{	
	DWORD last_error = 0;
	bool result = false;

	SERVICE_STATUS svc_status;
	memset(&svc_status, 0x00, sizeof(SERVICE_STATUS));

	std::cout << "Stoping service ";

	if (::ControlService(svc_handle, SERVICE_CONTROL_STOP, &svc_status)) {

		DWORD state = 0;
		DWORD start = ::GetTickCount();

		while (state = ServiceState(svc_handle)) {
			std::cout << ".";
			if (state == SERVICE_STOPPED) {
				std::cout << "\nService stoped.\n";
				result = true;
				break;
			}
			if ((::GetTickCount() - start) > 30 * 1000) {
				std::cout << "\nError: service stop timeout reached.\n";
				result = false;
				break;
			}
			::Sleep(500);
		}	
	}

	if (!result) {
		last_error = ::GetLastError();
		std::cout << "\nError in TryStopService(). Error code: " << last_error << ".\n";
	}
	return result;
}

bool TryRestartService(const TCHAR *svc_name)
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

	SC_HANDLE svc_handle = OpenService(scm_handle, svc_name, SERVICE_ALL_ACCESS);
	if (svc_handle == NULL){
		std::cout << "OpenService failed: " << ::GetLastError() << "\n";
		CloseServiceHandle(scm_handle);
		return false;
	}

	TryStopService(svc_handle);
	TryRunService(svc_handle);

	return true;
}

bool InstallService(const TCHAR *svc_name)
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

	std::cout << "Connected to SCM \n";

	SC_HANDLE svc_handle = CreateService(scm_handle,
		SERVICE_NAME, SERVICE_DISPLAY_NAME,
		SERVICE_ALL_ACCESS,
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

	TryRunService(svc_handle);

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

	SC_HANDLE svc_handle = OpenService(scm_handle, svc_name, SERVICE_ALL_ACCESS);
	if (svc_handle == NULL){
		std::cout << "OpenService failed: " << ::GetLastError() << "\n";
		CloseServiceHandle(scm_handle);
		return false;
	}

	TryStopService(svc_handle);

	if (!DeleteService(svc_handle)) {
		std::cout << "DeleteService failed: " << GetLastError() << "\n";
	} else {
		std::cout << "Service deleted.\n";
	} 

	::CloseServiceHandle(svc_handle);
	::CloseServiceHandle(scm_handle);

	return true;
}

bool CurrentDirectory(std::wstring &current_directory)
{
	std::vector<wchar_t> buff;
	DWORD ret = ::GetCurrentDirectoryW(0, NULL);
	if (ret) {
		buff.resize(ret + 1);
		if (::GetCurrentDirectoryW(buff.size(), buff.data())) {
			current_directory = buff.data();
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

	std::cout << "\n";

	if (CurrentDirectory(current_directory)) {

		service_module_path.append(current_directory + L"\\GuardSystemSvc.exe");

		if (!FileExists(service_module_path)) {
			std::wcout << "Service module not found." << L"(" << service_module_path << L").";
		} else if (argc == 1) {
			result = InstallService(service_module_path.c_str());
		} else if (argc >= 2) {
			std::wstring argument = argv[1];
			if (argument == _T("install")) {
				InstallService(service_module_path.c_str());
			} else if (argument == _T("uninstall")) {
				std::wstring svc_name = argv[1];
				DeleteService(SERVICE_NAME);
			} else if (argument == _T("restart")) {
				TryRestartService(SERVICE_NAME);
			}
			else {
				std::cout << "Wrong 1st parameter. Must be [install] or [uninstall]\n";
			}
		}
	}

	std::cout << "\n";
	::system("pause");

	return 0;
}

