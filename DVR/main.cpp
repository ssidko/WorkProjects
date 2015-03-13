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
	std::string volume_name("H:\\37025-DHFS\\origianl.dsk");

	LONGLONG offset = (LONGLONG)/*500*1024*1024*1024*/0;
	Timestamp min_time(2014,1,1,0,0,0);
	Timestamp max_time(2014,12,15,0,0,0);
	FrameSequenceInfo sequence_info;
	FileStorage storage(16, raw_directory_path, mkv_directory_path);
	std::vector<BYTE> sequence_buffer;
	sequence_buffer.reserve(4*1024*1024);
	Volume volume(volume_name);
	if (volume.Open()) {
		//volume.SetPointer(offset);
		//volume.Test();
		while (volume.NextFrameSequence(sequence_buffer, sequence_info)) {
			if ((sequence_info.start_frame.timestamp > min_time) && ((sequence_info.start_frame.timestamp < max_time))) {
				if (!storage.SaveFrameSequence(sequence_buffer, sequence_info)) {
					break;
				}
			}
		};
	}

	w.show();
	return a.exec();
}
