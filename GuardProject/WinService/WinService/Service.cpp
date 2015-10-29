#include "stdafx.h"
#include "Service.h"
#include <Dbt.h>

#include <fstream>

std::fstream log_file;

SERVICE_STATUS svc_status;
SERVICE_STATUS_HANDLE svc_status_handle;
HDEVNOTIFY notify_handle = NULL;

void ServiceRun(void)
{
	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);

	log_file.open("D:\\GitHub\\WorkProjects\\GuardProject\\WinService\\Debug\\log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	if (log_file.is_open()) {
		log_file << "Service started.\n";
	}
	log_file.close();
}

void WINAPI ServiceMain(DWORD args_count, LPWSTR *args)
{
	svc_status_handle = RegisterServiceCtrlHandlerEx(SERVICE_NAME, ServiceControlHandlerEx, NULL);
	if (!svc_status_handle) {
		// Error handling;
		return;
	}
	
	memset(&svc_status, 0x00, sizeof(svc_status));
	svc_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	svc_status.dwServiceSpecificExitCode = 0;

	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	DEV_BROADCAST_DEVICEINTERFACE filter;
	memset(&filter, 0x00, sizeof(filter));
	filter.dbcc_size = sizeof(filter);
	filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;

	notify_handle = ::RegisterDeviceNotification(svc_status_handle, &filter, DEVICE_NOTIFY_SERVICE_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
	if (notify_handle) {
		log_file.open("D:\\GitHub\\WorkProjects\\GuardProject\\WinService\\Debug\\log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
		if (log_file.is_open()) {
			log_file << "Notification filter registered.\n";
		}
		log_file.close();
	} else {
		DWORD error_code = ::GetLastError();
		log_file.open("D:\\GitHub\\WorkProjects\\GuardProject\\WinService\\Debug\\log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
		if (log_file.is_open()) {
			log_file << "Notification filter registration fail. Error code: " << error_code << "\n";
		}
		log_file.close();
	}

	ServiceRun();
}

DWORD WINAPI ServiceControlHandlerEx(DWORD  control_code, DWORD  event_type, LPVOID event_data, LPVOID context)
{
	switch (control_code) {
		case SERVICE_CONTROL_INTERROGATE:
			break;

		case SERVICE_CONTROL_CONTINUE:
			ReportSvcStatus(SERVICE_CONTINUE_PENDING, NO_ERROR, 0);

			ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
			break;

		case SERVICE_CONTROL_PAUSE:
			ReportSvcStatus(SERVICE_PAUSE_PENDING, NO_ERROR, 0);

			ReportSvcStatus(SERVICE_PAUSED, NO_ERROR, 0);
			break;

		case SERVICE_CONTROL_STOP:
			ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);

			if (notify_handle) {
				::UnregisterDeviceNotification(notify_handle);
			}

			log_file.open("D:\\GitHub\\WorkProjects\\GuardProject\\WinService\\Debug\\log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
			if (log_file.is_open()) {
				log_file << "Service stoped.\n";
			}
			log_file.close();

			ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
			break;

		case SERVICE_CONTROL_DEVICEEVENT:
			log_file.open("D:\\GitHub\\WorkProjects\\GuardProject\\WinService\\Debug\\log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
			if (log_file.is_open()) {
				log_file << "SERVICE_CONTROL_DEVICEEVENT\n";
			}
			log_file.close();
			break;

		default:
			return ERROR_CALL_NOT_IMPLEMENTED;
			break;
	}
	return NO_ERROR;
}

void ReportSvcStatus(DWORD current_state, DWORD win32_exit_code, DWORD wait_hint)
{
	static DWORD check_point = 1;

	svc_status.dwCurrentState = current_state;
	svc_status.dwWin32ExitCode = win32_exit_code;
	svc_status.dwWaitHint = wait_hint;

	if (current_state == SERVICE_START_PENDING) {
		svc_status.dwControlsAccepted = 0;
	} else {
		svc_status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	} 

	if ((current_state == SERVICE_RUNNING) || (current_state == SERVICE_STOPPED)) {
		svc_status.dwCheckPoint = 0;
	} else {
		svc_status.dwCheckPoint = check_point++;
	}

	// Report the status of the service to the SCM.
	SetServiceStatus(svc_status_handle, &svc_status);
}
