#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include "VHDFile.h"
#include "VDIFile.h"
#include "DiskImageExtractor.h"
#include "BitMap.h"
#include "DiskImageFile.h"


int main(int argc, char *argv[])
{
	VDIFile vdisk(QString::fromLocal8Bit("E:\\36573\\Maga\\Maga.vdi"));
	if (vdisk.Open()) {
		int x = 0;
		x = 123;
		x++;
	}

	int x = DiskImageFile::FileFormat::kVdiFile;

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
