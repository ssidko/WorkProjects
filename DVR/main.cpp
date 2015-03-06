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

	//std::string raw_directory_path = "E:\\37025\\h264";
	//std::string mkv_directory_path = "E:\\37025\\mkv";

	//LONGLONG offset = 0x6252E1D400ll;
	//W32Lib::FileEx dhfs_volume(_T("H:\\37025\\origianl.dsk"));
	//if (dhfs_volume.Open() && dhfs_volume.SetPointer(offset)) {
	//	Timestamp min_time(2014,1,1,0,0,0);
	//	Timestamp max_time(2014,12,15,0,0,0);
	//	FrameSequence sequence;
	//	FileStorage storage(16, raw_directory_path, mkv_directory_path);
	//	while (Frame::NextFrameSequence(dhfs_volume, sequence)) {
	//		if ((sequence.start_time > min_time) && ((sequence.start_time < max_time))) {
	//			if (!storage.SaveFrameSequence(dhfs_volume, sequence)) {
	//				break;
	//			}
	//		}
	//	}
	//}

	std::string file_name = "H:\\37025-DHFS\\origianl.dsk";
	LONGLONG offset = 13711880LL*512;
	LONGLONG ptr = 0;
	LONGLONG ptr1=offset;
	LONGLONG ptr2=offset + 512;
	LONGLONG ptr3=offset - 512;
	LONGLONG ptr4=offset + 512 + 512;
	DWORD rw = 0;
	DWORD size = 512;
	std::vector<BYTE> buffer(size);
	BufferedFile file(file_name);

	if (file.Open()) {
		//file.SetPointer(offset);
		//rw = file.Read(buffer.data(), size); 
		//	
		//file.SetPointer(offset - 512);
		//rw = file.Read(buffer.data(), size); 

		//file.SetPointer(offset + 512);
		//rw = file.Read(buffer.data(), size);

		//file.SetPointer(offset + 512 + 512);
		//rw = file.Read(buffer.data(), size);

		file.SetPointer(ptr1);
		file.Pointer(ptr);
		file.SetPointer(ptr1);
		file.Pointer(ptr);

		file.SetPointer(ptr2);
		file.Pointer(ptr);
		file.SetPointer(ptr2);
		file.Pointer(ptr);

		file.SetPointer(ptr3);
		file.Pointer(ptr);
		file.SetPointer(ptr3);
		file.Pointer(ptr);

		file.SetPointer(ptr4);
		file.Pointer(ptr);
		file.SetPointer(ptr3);
		file.Pointer(ptr);

		rw = 0;
	}

	w.show();
	return a.exec();
}
