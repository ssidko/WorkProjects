#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include "VHDFile.h"

int main(int argc, char *argv[])
{
	bool result;
	VHDFile vhd("J:\\test.vhd");
	QFile out("J:\\1.bin");
	char *buff = NULL;
	if (vhd.Open() && out.open(QIODevice::ReadWrite)) {
		buff = new char[vhd.BlockSize()];
		DWORD i = 0;
		while (vhd.ReadBlock(i, buff)) {
			out.write(buff, (qint64)vhd.BlockSize());
			i++;
		}
		int x = 0;
	}

	QApplication a(argc, argv);
	VHDExtractor w;
	w.show();
	return a.exec();
}
