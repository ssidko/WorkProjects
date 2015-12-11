#ifndef _SCMANAGER_H
#define _SCMANAGER_H

#include <windows.h>
#include "MyError.h"

class SCManager
{
private:
	SC_HANDLE handle;
	MyLib::WinError last_error;
	SC_HANDLE OpenService(const TCHAR *service_name);
public:
	SCManager();
	~SCManager();
	bool Open(DWORD desired_access = GENERIC_READ/*SC_MANAGER_ALL_ACCESS*/);
	void Close(void);
	MyLib::WinError LastError(void) { return last_error; }
};

#endif // _SCMANAGER_H

