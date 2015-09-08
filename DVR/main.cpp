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
	

	WFS::Main();
	//w.show();

	//TestWidget w;
	//w.setWindowTitle(QString("Test Widget"));
	//w.show();

	//int block_size = 64 * 4096;
	//BYTE *block = new BYTE[block_size];
	//LONGLONG start_block = 0;
	//LONGLONG blocks_count = 3146388LL;

	//std::vector<std::basic_string<TCHAR>> file_names;
	//file_names.push_back(_T("K:\\38270\\#06.img"));
	//file_names.push_back(_T("K:\\38270\\#00.img"));
	//file_names.push_back(_T("K:\\38270\\#01.img"));
	//file_names.push_back(_T("K:\\38270\\#02.img"));
	//file_names.push_back(_T("K:\\38270\\#03.img"));
	//file_names.push_back(_T("K:\\38270\\#04.img"));
	//file_names.push_back(_T("K:\\38270\\#05.img"));
	//Raid5 file_raid(file_names, MyLib::FileMode::kReadOnly, block_size, kLeftAsymmetric);

	//std::vector<std::basic_string<TCHAR>> disk_names;
	//disk_names.push_back(_T("\\\\.\\PhysicalDrive5"));
	//disk_names.push_back(_T("\\\\.\\PhysicalDrive0"));
	//disk_names.push_back(_T("\\\\.\\PhysicalDrive1"));
	//disk_names.push_back(_T("\\\\.\\PhysicalDrive2"));
	//disk_names.push_back(_T("\\\\.\\PhysicalDrive3"));
	//disk_names.push_back(_T("\\\\.\\PhysicalDrive4"));
	//Raid5 disk_raid(disk_names, MyLib::FileMode::kReadWrite, block_size, kLeftAsymmetric);

	//if (file_raid.Open() && disk_raid.Open()) {
	//	for (LONGLONG block_number = start_block; block_number <= blocks_count; ++block_number) {
	//		if (file_raid.ReadBlock(block_number, block)) {
	//			disk_raid.WriteBlock(block_number, block);
	//		}		
	//	}
	//}

	//MyLib::WinFile out_file(_T("K:\\38270\\test.bin"));
	//start_block = 109366825LL;
	//if (disk_raid.Open() && out_file.Create(MyLib::FileMode::kReadWrite)) {
	//	for (LONGLONG block_number = start_block; block_number <= (start_block + 1000); ++block_number) {
	//		if (disk_raid.ReadBlock(block_number, block)) {
	//			out_file.Write(block, block_size);
	//		}		
	//	}
	//}

	return a.exec();
}
