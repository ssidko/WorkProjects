#ifndef _SERVICE_H
#define _SERVICE_H

#include "tchar.h"
#include "windows.h"
#include "SvcGeneral.h"
#include <string>
#include "MasterModule.h"

bool ServiceInitialize(DWORD args_count, LPWSTR *args);
void ServiceDeinitialize(void);
void ServiceRun(void);
void ProcessMessage(Message &msg);
void WINAPI ServiceMain(DWORD args_count, LPWSTR  *args);
DWORD WINAPI ServiceControlHandlerEx(DWORD  control_code, DWORD  event_type, LPVOID event_data, LPVOID context);
void ReportSvcStatus(DWORD current_state, DWORD win32_exit_code, DWORD wait_hint);
bool ExecuteScript(std::string &script_file);

#endif // _SERVICE_H