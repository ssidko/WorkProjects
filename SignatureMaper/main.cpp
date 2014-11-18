#include "MainWindow.h"
#include <QtWidgets/QApplication>

#include "PhysicalDrive.h"
#include "Signature.h"

#include <fstream>
#include <iostream>

using namespace std;

void CreateSignatureMap(
		char *disk_name,
		unsigned int start_sector,
		unsigned int sector_count,
		const char  *out_file,
		unsigned int sector_size = 512,
		unsigned int io_block_size = 256*512)
{
	std::fstream disk(disk_name, ios_base::in|ios_base::out|ios_base::binary);
	unsigned int sector_counter = 0;
	unsigned int sectors_readed = 0;
	unsigned int rw = 0;

	char *buffer = new char[io_block_size];
	char *sector = buffer;

	if (disk.is_open()) {
		do {
			memset(buffer, 0x00, io_block_size);
			disk.read(buffer, io_block_size);
			for (sector = buffer; sector < buffer + io_block_size; sector += sector_size) {



				++sector_counter;
			}
		} while (!disk.good())
	}
}


int main(int argc, char *argv[])
{
	std::fstream io("\\\\.\\PhysicalDrive1", std::ios_base::in|std::ios_base::out|std::ios_base::binary);
	char buffer[512] = {0x00};
	if (io.is_open()) {
		io.read(buffer, 512);

		memset(buffer, 0xFF, 512);
		io.seekg(0);
		io.write(buffer, 512);
		io.flush();

		io.seekg(0);
		io.read(buffer, 512);

		io.close();
	}

	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
