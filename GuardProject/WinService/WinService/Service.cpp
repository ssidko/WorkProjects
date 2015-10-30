#include "stdafx.h"
#include "Service.h"
#include <Dbt.h>
#include <fstream>
#include "LogFile.h"

#define LOG_FILE_NAME			"GuardSystem.log"

LogFile log_file;
SERVICE_STATUS svc_status;
SERVICE_STATUS_HANDLE svc_status_handle;
HDEVNOTIFY notify_handle = NULL;

DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);

bool ServiceInitialize(DWORD args_count, LPWSTR *args)
{

	std::string service_file_path;
	service_file_path.resize(MAX_PATH);
	if (::GetModuleFileNameA(NULL, &service_file_path[0], service_file_path.length()) == 0x00) {
		return false;
	}

	int pos = (service_file_path.length() - 1);
	while (pos-- >= 0) {
		if (service_file_path[pos] == '\\') {
			break;
		}	
	}
	if (!pos) {
		return false;
	}

	std::string log_file_name(service_file_path, 0, pos + 1);
	log_file_name += LOG_FILE_NAME;

	log_file.SetOutFile(log_file_name.c_str());

	log_file << "\n" << "*** Service initialization *** \n";

	log_file.PrintLine("Log file name: %s", log_file_name.c_str());

	DEV_BROADCAST_DEVICEINTERFACE filter;
	memset(&filter, 0x00, sizeof(filter));
	filter.dbcc_size = sizeof(filter);
	filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	filter.dbcc_classguid = GUID_DEVINTERFACE_COMPORT;

	notify_handle = ::RegisterDeviceNotification(svc_status_handle, &filter, DEVICE_NOTIFY_SERVICE_HANDLE /*| DEVICE_NOTIFY_ALL_INTERFACE_CLASSES*/);
	if (notify_handle) {
		log_file << "Notification filter registered\n";
	}
	else {
		DWORD error_code = ::GetLastError();
		log_file.PrintLine("Notification filter registration fail. Error code: %d", error_code);
		return false;
	}
	
	return true;
}

void ServiceDeinitialize(void)
{
	if (notify_handle) {
		::UnregisterDeviceNotification(notify_handle);
	}
}

void ServiceRun(void)
{
	ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
	log_file << "*** Service started ***\n";

}

void WINAPI ServiceMain(DWORD args_count, LPTSTR *args)
{
	DWORD last_error;
	svc_status_handle = RegisterServiceCtrlHandlerEx(SERVICE_NAME, ServiceControlHandlerEx, NULL);
	if (!svc_status_handle) {
		return;
	}

	memset(&svc_status, 0x00, sizeof(svc_status));
	svc_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	svc_status.dwServiceSpecificExitCode = 0;

	ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 1000);

	if (ServiceInitialize(args_count, args)) {
		ServiceRun();
	} else {
		last_error = ::GetLastError();
		ReportSvcStatus(SERVICE_STOPPED, last_error, 0);
	}

}

DWORD WINAPI ServiceControlHandlerEx(DWORD  control_code, DWORD  event_type, LPVOID event_data, LPVOID context)
{
	//PDEV_BROADCAST_HDR broadcast_hader;
	//PDEV_BROADCAST_PORT broadcast_port;
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
			ServiceDeinitialize();
			log_file << "*** Service stoped ***\n";
			ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
			break;

		case SERVICE_CONTROL_DEVICEEVENT:
			switch (event_type) {
				case DBT_DEVICEARRIVAL:
					log_file << "DBT_DEVICEARRIVAL\n";
					break;
				case DBT_DEVICEREMOVECOMPLETE:
					log_file << "DBT_DEVICEREMOVECOMPLETE\n";
					break;
				default:
					break;
			}
			break;

		case SERVICE_CONTROL_SHUTDOWN:

			ServiceDeinitialize();
			log_file << "*** System shutdown ***\n";
			ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
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
		svc_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	} 

	if ((current_state == SERVICE_RUNNING) || (current_state == SERVICE_STOPPED)) {
		svc_status.dwCheckPoint = 0;
	} else {
		svc_status.dwCheckPoint = check_point++;
	}

	// Report the status of the service to the SCM.
	SetServiceStatus(svc_status_handle, &svc_status);
}
