#include "stdafx.h"
#include <windows.h>
#include "Service.h"
#include "SvcGeneral.h"

// Testing section
#include "LogFile.h"

void Test(void)
{
	LogFile log_file;
	log_file.SetOutFile("d:\\test.log");
	log_file.PrintLine("Testing LogFile class.");
	log_file.PrintLine("value: %d", 100);
	log_file.PrintLine("LogFile::PrintLine() value %d", -1);
}

int _tmain(int argc, _TCHAR* argv[])
{
	Test();

	SERVICE_TABLE_ENTRY dispatch_table[] = {
		{SERVICE_NAME, ServiceMain},
		{NULL, NULL}
	};

	StartServiceCtrlDispatcher(dispatch_table);

	return 0;
}