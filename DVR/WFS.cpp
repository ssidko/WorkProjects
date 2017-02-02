#include "WFS.h"

#include <iostream>
#include "utility.h"

#include "File.h"

using namespace WFS;

//void ConvertToMkv(std::string &raw_file_name, std::string &mkv_file_name)
//{
//	std::string t;
//	//std::string convertor_app("C:\\Program Files\\MKVToolNix\\mkvmerge.exe ");
//	std::string convertor_app("D:\\Soft\\#RecoverySoft#\\mkvtoolnix\\mkvmerge.exe ");
//
//	std::stringstream cmd_line;
//	cmd_line << convertor_app << "-o " << mkv_file_name << " " << raw_file_name;
//	t = cmd_line.str();
//	const char *zzz = t.data();
//	system(cmd_line.str().data());
//}

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
	sstream << sequence.start_time.ToString();
	sstream << "_" << sequence.offset;
	return sstream.str();
}

int WFS::Main(void)
{
	std::string wfs_file_name = "\\\\.\\PhysicalDrive0";
	//std::string wfs_file_name = "F:\\37566\\1.h264";
	std::string out_dir_path = "d:\\work\\38914\\";
	std::string mkv_file_name;
	std::string raw_file_name = out_dir_path + "out.dvr";

	Timestamp min_date(2015, 10, 1, 0, 0, 0);
	Timestamp max_date(2015, 10, 16, 0, 0, 0);

	//DWORD dw = 0x0e8e2cb9;
	//WFS::TIMESTAMP *t = (TIMESTAMP *)&dw;
	
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
				//raw_file_name = out_dir_path + SequenceInfoString(sequence) + ".h264";
				mkv_file_name = out_dir_path + SequenceInfoString(sequence) + ".avi";
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
				//ConvertToMkv(raw_file_name, mkv_file_name);
				Convert2Avi(raw_file_name, mkv_file_name);

			}
		}
		if (out_file && out_file->GetSize()) {
			out_file->Close();
			delete out_file;
			out_file = nullptr;
			//ConvertToMkv(raw_file_name, mkv_file_name);
			Convert2Avi(raw_file_name, mkv_file_name);
		}
	}

	return 0;
}

int WFS::StartRecovery(const std::string &volume_name, const std::string &out_directory, const Timestamp &start_time, const Timestamp &end_time)
{
	std::string mkv_file_name;
	std::string raw_file_name = out_directory + "out.dvr";

	Scanner volum(volume_name);
	W32Lib::FileEx *out_file = nullptr;

	LONGLONG max_offset = 0;

	if (volum.Open()) {
		FrameSequence sequence;
		//volum.SetPointer(1911834175387LL);
		while (volum.NextFrameSequence(sequence)) {

			if (start_time.Seconds()) {
				if (sequence.start_time.Seconds() < start_time.Seconds()) {
					if (sequence.end_time.Seconds() < start_time.Seconds()) {
						continue;
					}
				}
			}

			if (end_time.Seconds()) {
				if (sequence.start_time.Seconds() > end_time.Seconds()) {
					continue;
				}
			}

			if (out_file == nullptr) {
				//raw_file_name = out_dir_path + SequenceInfoString(sequence) + ".h264";
				mkv_file_name = out_directory + SequenceInfoString(sequence) + ".avi";
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
				//ConvertToMkv(raw_file_name, mkv_file_name);
				Convert2Avi(raw_file_name, mkv_file_name);

			}
		}
		if (out_file && out_file->GetSize()) {
			out_file->Close();
			delete out_file;
			out_file = nullptr;
			//ConvertToMkv(raw_file_name, mkv_file_name);
			Convert2Avi(raw_file_name, mkv_file_name);
		}
	}

	return 0;
}