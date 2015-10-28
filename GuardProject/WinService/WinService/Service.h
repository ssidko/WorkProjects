#ifndef _SERVICE_H
#define _SERVICE_H

#include "windows.h"
#include "SvcGeneral.h"

void WINAPI ServiceMain(DWORD args_count, LPWSTR  *args);
void WINAPI ServiceCtrlHandler(DWORD control_code);


class Service
{
public:
	Service();
	~Service();
};

#endif // _SERVICE_H

