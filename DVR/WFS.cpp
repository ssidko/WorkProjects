#include "WFS.h"

#include <iostream>
//#include "WinConsole.h"

#include "File.h"

using namespace WFS;

void ConvertToMkv(std::string &raw_file_name, std::string &mkv_file_name)
{
	std::string t;
	//std::string convertor_app("C:\\Program Files\\MKVToolNix\\mkvmerge.exe ");
	std::string convertor_app("D:\\Soft\\#RecoverySoft#\\mkvtoolnix\\mkvmerge.exe ");

	std::stringstream cmd_line;
	cmd_line << convertor_app << "-o " << mkv_file_name << " " << raw_file_name;
	t = cmd_line.str();
	const char *zzz = t.data();
	system(cmd_line.str().data());
}

void NextFileName(FrameSequence &sequence)
{

}

std::string WFS::SequenceInfoString(FrameSequence &sequence)
{
	std::stringstream sstream;
	//sstream << sequence.offset;
	//sstream << "\t( " << sequence.start_time.String() << " --- " << sequence.end_time.String() << " )";
	//sstream << "\tframe_cnt: " << sequence.frame_counter;
	//sstream << "\tsize: " << sequence.size;
	//sstream << "\n";
	//sstream << sequence.offset << "_";
	sstream << sequence.start_time.String();
	sstream << "_" << sequence.offset;
	return sstream.str();
}

int WFS::Main(void)
{
	std::string wfs_file_name = "\\\\.\\PhysicalDrive5";
	//std::string wfs_file_name = "F:\\37566\\1.h264";
	std::string out_dir_path = "K:\\38155\\";
	std::string mkv_file_name;
	std::string raw_file_name = out_dir_path + "out.dvr";

	Timestamp min_date(2015, 01, 01, 00, 00, 00);
	Timestamp max_date(2015, 12, 31, 00, 00, 00);

	
	DWORD dw = 0x3CF9788B;
	WFS::TIMESTAMP *t = (TIMESTAMP *)&dw;

	
	Scanner wfs(wfs_file_name);
	W32Lib::FileEx *out_file = nullptr;

	LONGLONG max_offset = 0;
	//WinConsole<TCHAR> console;
	
	if (wfs.Open()) {
		FrameSequence sequence;
		//wfs.SetPointer((1300) * 1024 * 1024 * 1024LL);
		while (wfs.NextFrameSequence(sequence)) {

			if ((sequence.start_time.Seconds() < min_date.Seconds()) || (sequence.start_time.Seconds() > max_date.Seconds())) {
				continue;
			}

			if (out_file == nullptr) {
				raw_file_name = out_dir_path + SequenceInfoString(sequence) + ".h264";
				mkv_file_name = out_dir_path + SequenceInfoString(sequence) + ".mkv";
				out_file = new W32Lib::FileEx(raw_file_name.data());
				if (out_file) {
					if (!out_file->Create()) {
						return -1;
					}
				}
			}

			out_file->Write(sequence.buffer.data(), sequence.buffer.size());

			if (out_file->GetSize() > 512 * 1024 * 1024LL) {
				out_file->Close();
				delete out_file;
				out_file = nullptr;
				ConvertToMkv(raw_file_name, mkv_file_name);
			}
		}	
	}

	return 0;
}