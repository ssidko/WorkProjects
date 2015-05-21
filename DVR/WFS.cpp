#include "WFS.h"
#include "WfsScanner.h"

#include <iostream>
#include <string>
#include "WinConsole.h"

using namespace WFS;

int WFS::Main(void)
{
	WinConsole console;

	std::wstring offset_str;
	std::string wfs_file_name = "\\\\.\\PhysicalDrive3";
	Scanner wfs(wfs_file_name);
	
	if (wfs.Open()) {
		LONGLONG offset = 0;

		wfs.SetPointer(0LL);
		while (wfs.NextFrameHeader(offset)) {
			offset_str = std::to_wstring(offset) + L"\n";
			//console.Print(offset_str.c_str());
			int x = 0;
			wfs.SetPointer(++offset);
		}	
		
		return 0;
	}

	return 0;
}