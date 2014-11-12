#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include "PhysicalDrive.h"
#include "Signature.h"

using namespace W32Lib;

void CreateSignatureMap(
		unsigned int disk_number,
		unsigned int start_sector,
		unsigned int sector_count,
		const char  *out_file,
		unsigned int sector_size = 512,
		unsigned int io_block_size = 256*512)
{
	PhysicalDrive disk(disk_number);
	unsigned int sector_counter = 0;
	unsigned int rw = 0;

	char *buffer = new char[io_block_size];

	if (disk.Open()) {
		while (rw = disk.Read(buffer, io_block_size)) {
			for (int i = 0; i < rw/sector_size; i++) {

			
				sector_counter++;
			}
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
