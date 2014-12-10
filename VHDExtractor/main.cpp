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

	DBFile db("J:\\Work\\36749\\Base_rep\\Сектор_к1\\SC12528.DBF");
	db.Open();

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
