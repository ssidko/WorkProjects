#include "stdafx.h"
#include <windows.h>
#include "Service.h"
#include "SvcGeneral.h"

int _tmain(int argc, _TCHAR* argv[])
{
	SERVICE_TABLE_ENTRY svc_entry;
	memset(&svc_entry, 0x00, sizeof(SERVICE_TABLE_ENTRY));
	svc_entry.lpServiceName = SERVICE_NAME;
	svc_entry.lpServiceProc = ServiceMain;

	StartServiceCtrlDispatcher(&svc_entry);

	return 0;
}