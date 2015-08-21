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

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	//MainWindow w;

	//QString path = "D:\\";
	//QDir base_dir(path);

	//QFileInfoList info_list = base_dir.entryInfoList();
	//foreach(QFileInfo file_info, info_list) {
	//	QString curr_path = file_info.absoluteFilePath();
	//	int x = 0;
	//}
	

	//WFS::Main();
	//w.show();

	//TestWidget w;
	//w.setWindowTitle(QString("Test Widget"));
	//w.show();

	std::vector< std::basic_string<TCHAR> > disk_names;
	disk_names.resize(6);
	disk_names[0] = _T("\\\\.\\PhysicalDrive0");
	disk_names[1] = _T("\\\\.\\PhysicalDrive1");
	disk_names[2] = _T("\\\\.\\PhysicalDrive2");
	disk_names[3] = _T("\\\\.\\PhysicalDrive3");
	disk_names[4] = _T("\\\\.\\PhysicalDrive4");
	disk_names[5] = _T("\\\\.\\PhysicalDrive5");

	Raid5 raid(disk_names, 64 * 4096, kLeftAasymmetric);
	raid.Open();
	raid.Close();

	return a.exec();
}
