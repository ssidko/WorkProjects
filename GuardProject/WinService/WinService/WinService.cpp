#include "stdafx.h"
#include <windows.h>
#include "Service.h"
#include "SvcGeneral.h"

int _tmain(int argc, _TCHAR* argv[])
{
	SERVICE_TABLE_ENTRY dispatch_table[] = {
		{SERVICE_NAME, ServiceMain},
		{NULL, NULL}
	};

	StartServiceCtrlDispatcher(dispatch_table);

	return 0;
}