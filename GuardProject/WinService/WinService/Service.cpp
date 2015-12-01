#include "stdafx.h"
#include "Service.h"
#include <Dbt.h>
#include <fstream>
#include <vector>
#include "LogFile.h"
#include "ComPort.h"

#define LOG_FILE_NAME			"GuardSystem.log"
#define SCRIPT_LINE_1			"script1.bat"
#define SCRIPT_LINE_2			"script2.bat"
#define SCRIPT_LINE_3			"script3.bat"
#define SCRIPT_LINE_4			"script4.bat"
#define SCRIPT_LINE_5			"script5.bat"
#define SCRIPTS_COUNT			5


LogFile log_file;
SERVICE_STATUS svc_status;
SERVICE_STATUS_HANDLE svc_status_handle;
HDEVNOTIFY notify_handle = NULL;
std::vector<HANDLE> events;
std::vector<std::string> scripts;

#define EVENT_DEVICE_ARRIVAL	0
#define EVENT_DEVICE_REMOVE		1

DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);

bool ServiceInitialize(DWORD args_count, LPWSTR *args)
{
	DWORD error_code = 0;

	//
	// Log file initialization
	//

	std::string log_file_name;
	std::string service_directory;
	service_directory.resize(MAX_PATH);
	if (::GetModuleFileNameA(NULL, &service_directory[0], service_directory.length()) == 0x00) {
		return false;
	}

	int pos = (service_directory.length() - 1);
	while (pos-- >= 0) {
		if (service_directory[pos] == '\\') {
			break;
		}	
	}
	if (!pos) {
		return false;
	}

	service_directory.resize(pos + 1);

	log_file_name = service_directory + LOG_FILE_NAME;
	log_file.SetOutFile(log_file_name.c_str());

	log_file << "\n" << "*** Service initialization *** \n";
	log_file.PrintLine("Log file name: %s", log_file_name.c_str());

	//
	// Set work directory
	//

	//log_file.PrintLine("Work directory: %s", service_directory.c_str());
	//if (::SetCurrentDirectoryA(service_directory.c_str())) {
	//	log_file << "Set work directory seccessful. \n";
	//} else {
	//	log_file.PrintLine("Set work directory error. WinError code: %d", ::GetLastError());
	//}

	//
	// Scripts initialization
	//

	scripts.resize(SCRIPTS_COUNT);
	scripts[0] = service_directory + SCRIPT_LINE_1;
	scripts[1] = service_directory + SCRIPT_LINE_2;
	scripts[2] = service_directory + SCRIPT_LINE_3;
	scripts[3] = service_directory + SCRIPT_LINE_4;
	scripts[4] = service_directory + SCRIPT_LINE_5;

	log_file.PrintLine("Script for control line 1: %s", scripts[0].c_str());
	log_file.PrintLine("Script for control line 2: %s", scripts[1].c_str());
	log_file.PrintLine("Script for control line 3: %s", scripts[2].c_str());
	log_file.PrintLine("Script for control line 4: %s", scripts[3].c_str());
	log_file.PrintLine("Script for control line 5: %s", scripts[4].c_str());


	//
	// Notification initialization
	//

	DEV_BROADCAST_DEVICEINTERFACE filter;
	memset(&filter, 0x00, sizeof(filter));
	filter.dbcc_size = sizeof(filter);
	filter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	filter.dbcc_classguid = GUID_DEVINTERFACE_COMPORT;

	notify_handle = ::RegisterDeviceNotification(svc_status_handle, &filter, DEVICE_NOTIFY_SERVICE_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);
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

	std::string script = "D:\\arrival.bat";

	while (true) {
		ret = ::WaitForMultipleObjects(events.size(), events.data(), FALSE, INFINITE);
		switch (ret) {
			case WAIT_OBJECT_0 + EVENT_DEVICE_ARRIVAL:
				log_file << "Device arrival.\n";
				ExecuteScript(scripts[0]);
				::ResetEvent(events[EVENT_DEVICE_ARRIVAL]);
				break;

			case WAIT_OBJECT_0 + EVENT_DEVICE_REMOVE:
				log_file << "Device remove.\n";
				ExecuteScript(scripts[1]);
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
					::SetEvent(events[EVENT_DEVICE_ARRIVAL]);
					break;
				case DBT_DEVICEREMOVECOMPLETE:
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

bool ExecuteScript(std::string &script_file)
{
	STARTUPINFOA si = {0};
	PROCESS_INFORMATION pi = {0};

	si.cb = sizeof(si);

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	si.lpTitle = "GuardSystem console";

	std::string cmd_line = "/C " + script_file;
	if (!CreateProcessA("cmd.exe", &cmd_line[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
		log_file.PrintLine("Error when executing the script (%s). WinError code: %d.", script_file.c_str(), ::GetLastError());
		return false;		
	} else {
		log_file.PrintLine("Script executed successful (%s).", script_file.c_str());
		return true;
	}
}
