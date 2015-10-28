#include "stdafx.h"
#include "Service.h"

void WINAPI ServiceMain(DWORD args_count, LPWSTR  *args)
{
	SERVICE_STATUS_HANDLE status = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceControlsHandler);
	if (!status) {
		// Error
		return;
	}


}

void WINAPI ServiceControlsHandler(DWORD control_code)
{
	int x = 0;
}

Service::Service()
{
}

Service::~Service()
{
}
