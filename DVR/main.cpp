#include "mainwindow.h"
#include <QtWidgets/QApplication>

#include "dhfs.h"
#include "WFS.h"
#include "File.h"

#include <vector>
#include "BufferedFile.h"

#include "ZPool.h"
#include "VDev.h"

using namespace DHFS;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	WFS::Main();

	w.show();
	return a.exec();
}
