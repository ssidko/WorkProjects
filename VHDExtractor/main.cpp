#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include "VHDFile.h"
#include "VDIFile.h"
#include "VHDExtractor.h"
#include "BitMap.h"

int main(int argc, char *argv[])
{
	//VDIFile vdi(QString::fromLocal8Bit("K:\\36076\\backaps\\fs1C.vdi"));
	//QFile out_file(QString::fromLocal8Bit("G:\\out.bin"));
	//if (out_file.open(QIODevice::ReadWrite) && vdi.Open()) {
	//	DWORD block_size = vdi.BlockSize();
	//	DWORD block_count = vdi.BlocksCount();
	//	char *buff = new char[block_size];
	//	for (DWORD i = 0; i < block_count; i++) {
	//		memset(buff, 0x00, block_size);
	//		vdi.ReadBlock(i, buff);
	//		out_file.write(buff, block_size);
	//	}
	//}

	//VHDFile vhd1("G:\\Windows 2003 Std x86 Courier d2.vhd");
	//VHDFile vhd2("G:\\Windows 2003 Std x86 Courier d2_E869CDB8-12F2-4ECF-AEE2-F57A8A4D0B53.avhd");
	//VHDFile vhd3("G:\\Windows 2003 Std x86 Courier d2_81A3833A-CE7E-4C08-8FD2-4D902FBE6319.avhd");
	//VHDFile vhd4("G:\\Windows 2003 Std x86 Courier d2_949C6D63-4EA8-49A8-A6A1-9E6DAC8C9BA0.avhd");
	//if (vhd1.Open() && vhd2.Open() && vhd3.Open() && vhd4.Open()) {
	//	DWORD blocks = vhd4.UsedEntriesInBAT();
	//	ULONGLONG size = (ULONGLONG)blocks*vhd4.BlockSize();
	//	int x = 0; 
	//}

	QFile out_file(QString::fromLocal8Bit("G:\\222.bin"));
	if (out_file.open(QIODevice::ReadWrite)) {
		bool ret = false;
		ret = out_file.seek(10000000);
		int x = 64;
		qint64 rw = out_file.read((char *)&x, sizeof(x));
		rw = out_file.write((char *)&x, sizeof(x));

		int y = 0;
	}

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
