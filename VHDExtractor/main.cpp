#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include "VHDFile.h"

int main(int argc, char *argv[])
{
	bool result;
	VHDFile vhd("J:\\test.vhd");
	if (vhd.Open()) {
		
		int x = 0;
	}

	QApplication a(argc, argv);
	VHDExtractor w;
	w.show();
	return a.exec();
}
