#include "stdafx.h"
#include <windows.h>
#include "Service.h"
#include "SvcGeneral.h"

void Test(void)
{
	ServiceInitialize(0, 0);
	ServiceRun();
}

int _tmain(int argc, _TCHAR* argv[])
{
	//Test();

	SERVICE_TABLE_ENTRY dispatch_table[] = {
		{SERVICE_NAME, ServiceMain},
		{NULL, NULL}
	};

	StartServiceCtrlDispatcher(dispatch_table);

	return 0;
}