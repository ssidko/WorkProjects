#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include "VHDFile.h"
#include <QMessageBox>

int main(int argc, char *argv[])
{
	bool result;
	VHDFile vhd("J:\\test.vhd");
	QFile out("J:\\1.bin");
	char *buff = NULL;
	if (vhd.Open() && out.open(QIODevice::ReadWrite)) {
		buff = new char[vhd.BlockSize()];
		DWORD i = 0;
		//while (vhd.ReadBlock(i, buff)) {
		//	out.write(buff, (qint64)vhd.BlockSize());
		//	i++;
		//}
		int x = 0;
	}

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	//QFile disk("\\\\.\\PhysicalDrive0");
	//if (disk.open(QIODevice::ReadWrite/*QIODevice::ReadOnly*/)) {
	//	char buff[512] = {0};
	//	qint64 rw = disk.read(buff, 512);
	//	int x = 0;
	//	QMessageBox::information(NULL, "dlg", QString::fromLocal8Bit("Ok!"));
	//} else {
	//	QString error = disk.errorString();
	//	QMessageBox::information(NULL, "dlg", disk.errorString());
	//	int x = 0;
	//}

	//QString disk_name = "\\\\.\\PhysicalDrive" + QString::number(0, 10);
	//HANDLE hFile = CreateFileA(disk_name.toLocal8Bit(), GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,
	//							OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL|FILE_FLAG_NO_BUFFERING, NULL);
	//if (hFile != INVALID_HANDLE_VALUE) {
	//	QFile disk;
	//	if (disk.open((int)hFile, QIODevice::ReadWrite)) {
	//		int x = 0;
	//	}
	//}

	return a.exec();
}
