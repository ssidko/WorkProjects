#include "stdafx.h"
#include "Service.h"
#include <Dbt.h>
#include <fstream>
#include <vector>
#include "LogFile.h"
#include "ComPort.h"

#define LOG_FILE_NAME			"GuardSystem.log"

LogFile log_file;
SERVICE_STATUS svc_status;
SERVICE_STATUS_HANDLE svc_status_handle;
HDEVNOTIFY notify_handle = NULL;
std::vector<HANDLE> events;

#define EVENT_DEVICE_ARRIVAL	0
#define EVENT_DEVICE_REMOVE		1

DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);

bool ServiceInitialize(DWORD args_count, LPWSTR *args)
{
	DWORD error_code = 0;

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
		error_code = ::GetLastError();
		log_file.PrintLine("Notification filter registration fail.  WinError code: %d", error_code);
		return false;
	}

	events.resize(2);
	for (DWORD i = 0; i < events.size(); i++) {
		events[i] = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		if (events[i] == NULL) {
			error_code = ::GetLastError();
			log_file.PrintLine("CreateEvent error. Event ID: %d. WinError code: %d.", i,  error_code);
			return false;
		}
	}

	log_file << "Device events created.\n";

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

	std::list<std::string> com_ports;
	ComPort::AvailableComPorts(com_ports);

	log_file.PrintLine("Avaliable COM ports: %d", com_ports.size());
	auto it = com_ports.begin();
	while (it != com_ports.end()) {
		log_file.PrintLine("%s", (*it).c_str());
		it++;
	}

	//
	// После старта необходимо проверить подключено ли уже устройство.
	// Если подключено - ждать сообщения.
	// Иначе - ждать подключения устройства.
	//

	DWORD ret = 0;

	std::string bat_file = "cmd.exe /C D:\\arrival.bat";

	while (true) {
		ret = ::WaitForMultipleObjects(events.size(), events.data(), FALSE, INFINITE);
		switch (ret) {
			case WAIT_OBJECT_0 + EVENT_DEVICE_ARRIVAL:
				log_file << "Device arrival.\n";
				RunBatFile(&bat_file[0]);
				::ResetEvent(events[EVENT_DEVICE_ARRIVAL]);
				break;

			case WAIT_OBJECT_0 + EVENT_DEVICE_REMOVE:
				log_file << "Device remove.\n";
				RunBatFile(&bat_file[0]);
				::ResetEvent(events[EVENT_DEVICE_REMOVE]);
				break;

			default:
				break;
		}
	}
	log_file << "Exit from ServiceRun().\n";
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
					::SetEvent(events[EVENT_DEVICE_ARRIVAL]);
					break;
				case DBT_DEVICEREMOVECOMPLETE:
					log_file << "DBT_DEVICEREMOVECOMPLETE\n";
					::SetEvent(events[EVENT_DEVICE_REMOVE]);
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

DWORD RunBatFile(char *bat_file)
{
	DWORD error_code = 0;
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	si.lpTitle = "GuardSystem console";

	log_file.PrintLine("command line arguments: %s.", bat_file);

	if (!CreateProcessA("cmd.exe",   // No module name (use command line)
		bat_file,        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si, &pi)
		) {

		error_code = ::GetLastError();
		log_file.PrintLine("CreateProcessA error. WinError code: %d.", error_code);
		return -1;		
	}

	return 0;
}
