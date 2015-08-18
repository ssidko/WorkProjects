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

	TestWidget w;

	w.setWindowTitle(QString("Test Widget"));
	//w.setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
	//w.setFrameStyle(QFrame::Box | QFrame::Plain);

	w.show();


	return a.exec();
}
