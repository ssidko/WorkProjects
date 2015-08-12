#pragma once

#ifndef _EXPEREMENTAL_H
#define _EXPEREMENTAL_H

#include "stdafx.h"
//#include "stdio.h"
//#include "conio.h"

#include <iostream>

#include "windows.h"
#include <tchar.h>
#include "setupapi.h"
#include "devguid.h"

#pragma comment(lib, "Setupapi.lib")

void EnumerateDevicesInterfaces(const GUID* device_interface_guid);
void Testing(void);

#endif // _EXPEREMENTAL_H

