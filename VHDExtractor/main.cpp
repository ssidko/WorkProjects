#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include "VHDFile.h"
#include "VDIFile.h"
#include "VHDExtractor.h"
#include "BitMap.h"

int main(int argc, char *argv[])
{
	VDIFile vdi(QString::fromLocal8Bit("K:\\36076\\backaps\\fs1C.vdi"));
	QFile out_file(QString::fromLocal8Bit("G:\\out.bin"));
	if (out_file.open(QIODevice::ReadWrite) && vdi.Open()) {
		DWORD block_size = vdi.BlockSize();
		DWORD block_count = vdi.BlocksCount();
		char *buff = new char[block_size];
		for (DWORD i = 0; i < block_count; i++) {
			memset(buff, 0x00, block_size);
			vdi.ReadBlock(i, buff);
			out_file.write(buff, block_size);
		}
	}

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
