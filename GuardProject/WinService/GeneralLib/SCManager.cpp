#include "stdafx.h"
#include "SCManager.h"

SCManager::SCManager() : handle(NULL)
{
}

SCManager::~SCManager()
{
}

SC_HANDLE SCManager::OpenService(const TCHAR *service_name)
{

}

bool SCManager::Open(DWORD desired_access)
{
	handle = ::OpenSCManager(NULL, NULL, desired_access);
	if (!handle) {
		last_error.Update();
		return false;
	} else {
		return true;
	}
}

void SCManager::Close(void)
{
	if (handle) {
		::CloseServiceHandle(handle);
		handle = NULL;
	}
}
