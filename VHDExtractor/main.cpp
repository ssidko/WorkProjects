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
	//ISpVoice * pVoice = NULL;

	//if (FAILED(::CoInitialize(NULL)))
	//	return FALSE;

	//HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
	//if( SUCCEEDED( hr ) )
	//{
	//	hr = pVoice->Speak(L" Hello, world!",0, NULL );
	//	pVoice->Release();
	//	pVoice = NULL;
	//}

	//::CoUninitialize();
	//return TRUE;

	//VDIFile vdisk(QString::fromLocal8Bit("E:\\36573\\Maga\\Maga.vdi"));
	//if (vdisk.Open()) {
	//	int x = 0;
	//	x = 123;
	//	x++;
	//}

	//const char *error_str = NULL;
	//char buff[512] = {};
	//std::fstream disk;
	//disk.exceptions(std::ifstream::failbit|std::ifstream::badbit);
	//try {
	//	disk.open("\\\\.\\PhysicalDrive1", std::ios_base::binary|std::ios_base::in|std::ios_base::out);
	//	if (!disk.fail()) {
	//		disk.seekg(0);
	//		disk.read(buff, 512);
	//	}
	//	//if ()
	//} catch (std::ios_base::failure &err) {
	//	error_str = err.what();
	//}

	//int x = DiskImageFile::Format::kVdiFile;

	//DBFile db("G:\\Testing\\DH4418~.DBF");
	//if (db.Open()) {
	//	db.RepairBadRecords();
	//}

	RepairAllDbf(QString::fromLocal8Bit("G:\\Testing\\Сектор_к1"));
 
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
