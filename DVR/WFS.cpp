#include "WFS.h"
#include "WfsScanner.h"

#include <iostream>
#include <string>
#include "WinConsole.h"

#include "File.h"

using namespace WFS;

int WFS::Main(void)
{
	WinConsole console;

	std::wstring offset_str;
	std::string wfs_file_name = "\\\\.\\PhysicalDrive3";
	Scanner wfs(wfs_file_name);
	
	if (wfs.Open()) {
		FrameSequence sequence;

		wfs.SetPointer(0LL);
		while (wfs.NextFrameSequence(sequence)) {
			W32Lib::File out("1.h264");
			if (out.Create()) {
				out.Write(sequence.buffer.data(), sequence.buffer.size());
			}
			sequence.Clear();
		}	
		
		return 0;
	}

	return 0;
}