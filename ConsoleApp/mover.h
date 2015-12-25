#pragma once
#include <windows.h>
#include <tchar.h>
#include <iostream>

namespace mover {
	void PrintUsage(void);
}

int mover_main(TCHAR *file_name, TCHAR *out_dir);