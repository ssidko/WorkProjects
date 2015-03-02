#include "mainwindow.h"
#include <QtWidgets/QApplication>

#include "dhfs.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	DHFS::StartRecovering();

	w.show();
	return a.exec();
}
