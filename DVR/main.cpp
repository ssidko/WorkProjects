#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QDir>

#include <vector>
#include "File.h"
#include "BufferedFile.h"

#include "WinConsole.h"
#include "TestWidget.h"
#include "Raid5.h"
#include "Orbita.h"

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
#include "dhfs.h"
#include "WFS.h"

#include <iostream>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	//Orbita::Main("\\\\.\\PhysicalDrive0", "K:\\38702\\out");
	//WFS::Main();

	//DWORD raw = 0x103CB6A9;
	//DWORD raw = 0x102C034a;
	//HIKV::TIMESTAMP *t = (HIKV::TIMESTAMP *)&raw;

	//HIKV::TIMESTAMP tst = { 0,0,0,25,2,16 };
	//DWORD *qq = (DWORD *)&tst;
	//qq;

	//DWORD dw = 0x409EB74F;
	//WFS::TIMESTAMP *tm = (WFS::TIMESTAMP *)&dw;

	//Timestamp time;
	//time = (Timestamp)tst.TimeStamp();

	//HIKV::StartRecovering("\\\\.\\PhysicalDrive4", "F:\\39710\\out\\", Timestamp(2016, 01, 01, 0, 0, 0), Timestamp());
	//WFS::StartRecovering("\\\\.\\PhysicalDrive11", "F:\\39493\\out\\", Timestamp(2016, 01, 01, 0, 0, 0), Timestamp());

	//FileRecordRecoveryStart();

	//std::string out_file_name = "F:\\39389\\frames_info.txt";
	//HIKV::HikVolume vol("\\\\.\\PhysicalDrive13");
	//if (vol.Open()) {
	//	vol.SaveFramesInfoToFile(out_file_name);	
	//}

	DHFS::StartRecovering("\\\\.\\PhysicalDrive1", "F:\\39988\\out\\", Timestamp(2015, 1, 1, 0, 0, 0), Timestamp(0, 0, 0, 0, 0, 0));
	//DHFS::StartRecovering("\\\\.\\PhysicalDrive1", "F:\\39988\\out\\", Timestamp(2015, 5, 1, 0, 0, 0), Timestamp(2016, 8, 14, 0, 0, 0));
	//DHFS::Volume volume("\\\\.\\PhysicalDrive1");
	//if (volume.Open()) {
	//	volume.SaveFrameInfo("F:\\39988\\frames.txt");
	//	//volume.SaveFrameSequenceInfo("F:\\39988\\sequences.txt");
	//}

	//DHFS::StartRecovering("\\\\.\\PhysicalDrive4", "F:\\40052\\out\\", Timestamp(2016,05,01,0,0,0), Timestamp(0,0,0,0,0,0));
	//DHFS::Volume volume("\\\\.\\PhysicalDrive4");
	//if (volume.Open()) {
	//	volume.SaveFrameInfo("F:\\40052\\frames.txt");
	//	//volume.SaveFrameSequenceInfo("F:\\40052\\sequences.txt");
	//}

	w.show();
	return a.exec();
}
