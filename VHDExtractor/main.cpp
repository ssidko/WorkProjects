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
#include <Fat.h>
#include "PhysicalDrive.h"

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
	using namespace fat;
	
	LONGLONG offset = (LONGLONG)953727*512 + (LONGLONG)1144382*64*512;
	DWORD cluster_size = 64*512;

	PhysicalDrive disk("\\\\.\\PhysicalDrive0");
	if (disk.Open()) {
		char *buff = (char *)new char[cluster_size];
		disk.SetPointer(offset);
		while (cluster_size == disk.Read(buff, cluster_size)) {
			DIR_ENTRY *entry = (DIR_ENTRY *)buff;
			for (int i = 0; i < cluster_size/sizeof(DIR_ENTRY); i++) {
				if (i == 0x45) {
					int y = 0;
				}
				bool result = IsValidDirEntry((char *)entry);
				entry++;
			}
			int x = 0;

		}
	}

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
