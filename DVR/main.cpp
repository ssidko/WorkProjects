#include "mainwindow.h"
#include <QtWidgets/QApplication>

#include "dhfs.h"
#include "WFS.h"
#include "File.h"

#include <vector>
#include "BufferedFile.h"

#include "ZPool.h"
#include "VDev.h"

#include <QDir>

using namespace DHFS;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	QString path = "D:\\";
	QDir base_dir(path);

	QFileInfoList info_list = base_dir.entryInfoList();
	foreach(QFileInfo file_info, info_list) {
		QString curr_path = file_info.absoluteFilePath();
		int x = 0;
	}

	w.show();
	return a.exec();
}
