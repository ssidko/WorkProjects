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
#include "Orbita.h"

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
#include "FileRecordRecovery.h"
#include "Hikvision.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	//Orbita::Main("\\\\.\\PhysicalDrive0", "K:\\38702\\out");
	//WFS::Main();

	//DWORD raw = 0x103CB6A9;
	DWORD raw = 0x102080fa;
	HIKV::TIMESTAMP *t = (HIKV::TIMESTAMP *)&raw;

	HIKV::TIMESTAMP tst = { 0,0,0,25,2,16 };
	DWORD *qq = (DWORD *)&tst;
	qq;


	//Timestamp time;
	//time = (Timestamp)tst.TimeStamp();

	//DHFS::StartRecovering("\\\\.\\PhysicalDrive1", "F:\\39405\\out1\\", Timestamp(2015,11,25,0,0,0), Timestamp());
	//HIKV::StartRecovering("\\\\.\\PhysicalDrive12", "F:\\39389\\out\\", Timestamp(2015, 11, 25, 0, 0, 0), Timestamp());

	//FileRecordRecoveryStart();

	w.show();
	return a.exec();
}
