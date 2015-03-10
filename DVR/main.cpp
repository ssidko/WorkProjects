#include "mainwindow.h"
#include <QtWidgets/QApplication>

#include "dhfs.h"
#include "File.h"

#include <vector>
#include "BufferedFile.h"

using namespace DHFS;

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	std::string raw_directory_path = "E:\\37025\\dav";
	std::string mkv_directory_path = "E:\\37025\\mkv";
	std::string volume_name = "H:\\37025-DHFS\\origianl.dsk";

	LONGLONG offset = 0x6252E1D400ll;
	BufferedFile dhfs_volume(volume_name, 10*1024*1024);
	if (dhfs_volume.Open() && dhfs_volume.SetPointer(offset)) {
		Timestamp min_time(2014,1,1,0,0,0);
		Timestamp max_time(2014,12,15,0,0,0);
		FrameSequence sequence;
		FileStorage storage(16, raw_directory_path, mkv_directory_path);
		while (Frame::NextFrameSequence(dhfs_volume, sequence)) {
			if ((sequence.start_time > min_time) && ((sequence.start_time < max_time))) {
				if (!storage.SaveFrameSequence(dhfs_volume, sequence)) {
					break;
				}
			}
		}
	}

	w.show();
	return a.exec();
}
