#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include "VHDFile.h"
#include "VDIFile.h"
#include "DiskImageExtractor.h"
#include "BitMap.h"
#include "DiskImageFile.h"

#include <fstream>

#include <sapi.h>
#include "DBFile.h"
#include <QDir>
#include <QFile>

void RepairAllDbf(QString &path)
{
	QStringList files_list;
	QString file;
	QDir dir(path);
	QString dir_path = dir.absolutePath();
	if (dir.exists()) {
		QStringList filters;
		filters << "*.dbf";
		dir.setFilter(QDir::Files);
		dir.setNameFilters(filters);
		files_list = dir.entryList();
		for (int i = 0; i < files_list.size(); i++) {
			file = dir.absolutePath() + "/" + files_list[i];
			DBFile db(file.toLocal8Bit().data());
			if (db.Open()) {
				db.RepairBadRecords();
			}
			int x = 25;
		}
	}
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
