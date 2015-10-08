#include "mainwindow.h"
#include <QtWidgets/QApplication>

#include "dhfs.h"
#include "WFS.h"
#include "File.h"

#include <vector>
#include "BufferedFile.h"

//#include "ZPool.h"
#include "VDev.h"

#include <QDir>

#include "WinConsole.h"

#include "TestWidget.h"
#include "Raid5.h"

using namespace DHFS;

inline void _trace(char *format, ...)
{
	static char buffer[2048] = { 0 };
	DWORD rw = 0;

	va_list argptr;
	va_start(argptr, format);
	vsprintf_s(buffer, format, argptr);
	va_end(argptr);

	::WriteConsoleA(::GetStdHandle(STD_OUTPUT_HANDLE), buffer, strlen(buffer), &rw, NULL);
}

#include "ExtentRecovery.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//MainWindow w;

	ext4::ExtentSaver saver("E:\\OUT", "K:\\38505\\sda.img", (LONGLONG)503808LL * 512, 4096);
	if (saver.Open()) {
		LONGLONG block_num = 0;
		while (saver.NextExtentBlock(block_num)) {
		
			int x = 0;
		}
	
		int x = 0;
	}

	return a.exec();
}
