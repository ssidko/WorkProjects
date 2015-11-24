#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include "VHDInfoDialog.h" 

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

typedef struct _DVR_DATE {
	DWORD seconds:6;
	DWORD minutes:6;
	DWORD hours:5;
	DWORD day:5;
	DWORD month:4;
	DWORD year:6;	
} DVR_DATE;

int main(int argc, char *argv[])
{
	//VHDFile file(QString::fromLocal8Bit("K:\\testing-ntfs.vhd"));
	//if (file.Open()) {
	//
	//
	//	int x = 0;
	//}

	DWORD raw = 0x3e8879ed;
	//DWORD raw = 0x3e000000;
	DVR_DATE *date = (DVR_DATE *)&raw;


	QApplication a(argc, argv);
	//MainWindow w;
	//VHDInfoDialog vhd_info;

	//RepairAllDbf(QString::fromLocal8Bit("D:\\Work\\38790\\2010"));

	//vhd_info.show();
	//w.show();
	return a.exec();
}
