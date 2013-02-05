#pragma once
#include <Windows.h>
#include <assert.h>
#include "W32Lib.h"

BYTE GetVarint(BYTE *p, ULONGLONG *v);
int Sqliter_main();


class SQLiter
{
private:
	FileEx io;
public:
	SQLiter(TCHAR *file_name) : io(file_name) {}
	~SQLiter();

	BOOL Open();
	void Close();
};



