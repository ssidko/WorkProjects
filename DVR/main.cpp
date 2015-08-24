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

	std::vector<std::basic_string<TCHAR>> disk_names;
	disk_names.push_back(_T("e:\\00.bin "));
	disk_names.push_back(_T("e:\\01.bin "));
	disk_names.push_back(_T("e:\\02.bin "));
	disk_names.push_back(_T("e:\\03.bin "));

	BYTE block[512];
	BOOL result = false;
	Raid5 raid(disk_names, MyLib::FileMode::kReadWrite, sizeof(block), kLeftAsymmetric);
	if (raid.Open()) {
		for (int i = 0; i < 1000; i++) {
			memset(block, 0x00, sizeof(block));
			sprintf((char *)block, "block: %08d", i);
			result = raid.WriteBlock(i, block);
			result = false;
		}

	
		raid.Close();
	}

	return a.exec();
}
