#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <string>
#include <vector>
#include "File.h"

bool FindAndSaveAllExtents()
{
	std::string file_name = "F:\\43693\\vhd\\1CServ_Backup.vhdx.decryptor@cock.li";
	std::string out_dir = "F:\\43693\\recovery\\extents";
	size_t block_size = 512;
	std::vector<uint8_t> buffer(256 * block_size);

	W32Lib::FileEx src_file(file_name.c_str());
	if (!src_file.Open()) {
		return false;
	}
	
	size_t readed = 0;
	while (readed = src_file.Read(buffer.data(), buffer.size())) {
	
		size_t blocks = readed / block_size;
		for (int i = 0 ; i < blocks; i++) {
		
			
		
		}
	
	}

	return false;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	bool result = FindAndSaveAllExtents();

	w.show();
	return a.exec();
}
