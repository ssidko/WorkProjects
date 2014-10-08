#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include "VHDFile.h"
#include "VDIFile.h"
#include "DiskImageExtractor.h"
#include "BitMap.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
