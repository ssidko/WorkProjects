#ifndef _SERVICE_H
#define _SERVICE_H

#include "windows.h"

void WINAPI ServiceMain(DWORD dwNumServicesArgs, LPWSTR  *lpServiceArgVectors);


class Service
{
public:
	Service();
	~Service();
};

#endif // _SERVICE_H

