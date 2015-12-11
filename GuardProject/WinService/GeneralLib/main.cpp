#include "stdafx.h"
#include "SCManager.h"
#include "WinConsole.h"


int _tmain(int argc, _TCHAR* argv[])
{
	WinConsole console;
	SCManager scm;
	if (scm.Open(SC_MANAGER_ALL_ACCESS)) {
		console << _T("SCM opened.") << _T("\n");


	} else {
		console << scm.LastError().Description() << _T("\n");
	}

	return 0;
}

