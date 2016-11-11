#include "dvr1.h"
#include <ctime>

BOOL ForEachFileFolder(const char *path,
	FOREACHFILEPROC file_proc,
	FOREACHFILEPROC folder_proc,
	BOOL check_subfolders)
{
	if (!path || !(file_proc || folder_proc))
		return FALSE;

	std::string find_path = path;
	find_path += "\\*.*";

	WIN32_FIND_DATAA wfd = { 0 };
	HANDLE hFind = ::FindFirstFileA(find_path.data(), &wfd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			std::string file_name = wfd.cFileName;

			if ((file_name == ".") || (file_name == ".."))
				continue;

			std::string full_file_path = path;
			full_file_path += "\\";
			full_file_path += file_name;

			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (check_subfolders)
					ForEachFileFolder(full_file_path.c_str(), file_proc, folder_proc, check_subfolders);
				if (folder_proc)
					folder_proc(full_file_path);
			}
			else if (file_proc)
				file_proc(full_file_path);


		} while (::FindNextFileA(hFind, &wfd));

		::FindClose(hFind);
		return TRUE;
	}
	else
		return FALSE;
}

void ConvertToMkvA(std::string &raw_file_name, std::string &mkv_file_name)
{
	std::string t;
	//std::string convertor_app("C:\\Program Files\\MKVToolNix\\mkvmerge.exe ");
	std::string convertor_app("d:\\Soft\\MKVToolNix\\mkvmerge.exe ");

	std::stringstream cmd_line;
	cmd_line << convertor_app << "-o " << mkv_file_name << " " << raw_file_name;
	t = cmd_line.str();
	system(cmd_line.str().data());
}

void ConvertToAvi(std::string &raw_file_name, std::string &avi_file_name)
{
	// ffmpeg -f h264 -i m:\38646\fly01195.ifv -an -vcodec copy video.avi
	std::string cmd_line = "ffmpeg -f h264 -i ";
	cmd_line += raw_file_name;
	cmd_line += " -an -vcodec copy ";
	cmd_line += avi_file_name;

	system(cmd_line.data());
}

DWORD WINAPI ToMkv(std::string &file_path)
{
	std::string mkv_file_name = file_path + ".mkv";
	ConvertToMkvA(file_path, mkv_file_name);
	return 0;
}

DWORD WINAPI ToAvi(std::string &file_path)
{
	std::string avi_file_name = file_path + ".avi";
	ConvertToAvi(file_path, avi_file_name);
	return 0;
}

DWORD WINAPI Convert(std::string &file_path)
{
	ToMkv(file_path);
	ToAvi(file_path);
	return 0;
}

int dvr1::Run(const char *raw_files_path, const char *out_dir_path, const char *index_file_name)
{
	std::string path_to_raw = raw_files_path;
	std::string idx_file_name = index_file_name;
	std::string raw_file_name;
	std::string raw_file_path;
	std::vector<IndexRecord> idx_list;

	if (InitializeIndexList(index_file_name, idx_list)) {
		auto it = idx_list.begin();
		while (it != idx_list.end()) {
			FileName(*it, raw_file_name);
			raw_file_path = path_to_raw + "\\" + raw_file_name;

			ConvertFile(*it, raw_file_path, out_dir_path);

			it++;
		}		
	}
	return 0;
}

bool dvr1::InitializeIndexList(const std::string &index_file_name, std::vector<IndexRecord> &idx_list)
{
	BufferedFile index_file(index_file_name);
	if (index_file.Open()) {
		DWORD idx_count = 0;

		if (!index_file.SetPointer(16)) return false;
		if (index_file.Read((void *)&idx_count, sizeof(DWORD)) != sizeof(DWORD)) return false;
		
		idx_list.resize(idx_count);

		if (!index_file.SetPointer(512)) return false;
		for (int i = 0; i < idx_count; i++) {

			IndexRecord record;
			if (index_file.Read((void *)&record, sizeof(IndexRecord)) != sizeof(IndexRecord)) return false;

			if ((record.file_id <= idx_count) && (record.camera <= 16)) {
				idx_list[i] = record;
			}		
		}
		return true;
	}
	return false;
}

void dvr1::FileName(IndexRecord &record, std::string &file_name)
{
	char buff[64] = { 0 };
	sprintf_s(buff, sizeof(buff), "fly%05u.ifv", record.file_id);
	file_name = buff;
}

void dvr1::ConvertFile(IndexRecord &idx_record, std::string &raw_file_path, const std::string &out_dir)
{
	std::string start_time;
	std::string end_time;
	TimeToString(idx_record.start_time, start_time);
	TimeToString(idx_record.end_time, end_time);

	std::string out_file_path = out_dir + "\\" + "CAM-" + std::to_string(idx_record.camera) + "-" ;
	out_file_path += "[" + start_time + "]" + "--" + "[" + end_time + "]" + "-" + std::to_string(idx_record.file_id) + ".avi";

	std::string tmp_file_name = out_dir + "\\tmp";

	W32Lib::FileEx in_file(raw_file_path.c_str());
	W32Lib::FileEx tmp_file(tmp_file_name.c_str());

	if (in_file.Open() && tmp_file.Create()) {
		DWORD rw = 0;
		const int buff_size = 256 * 512;
		char *buff = new char[buff_size];


		BYTE sign[] = { 0x00, 0x00, 0x00, 0x01, 0x67, 0x42 };
		in_file.Find(sign, sizeof(sign));

		while (rw = in_file.Read(buff, buff_size)) {
			tmp_file.Write(buff, rw);
		}
		in_file.Close();
		tmp_file.Close();
	}

	ConvertToAvi(tmp_file_name, out_file_path);
}

void dvr1::TimeToString(DWORD time, std::string &str)
{
	time_t raw_time = (time_t)time;
	//tm *tmi = localtime(&raw_time);
	tm *tmi = gmtime(&raw_time);

	char buff[64] = { 0 };
	sprintf_s(buff, sizeof(buff), "%04d-%02d-%02d--%02d-%02d-%02d", tmi->tm_year + 1900, tmi->tm_mon + 1, tmi->tm_mday, tmi->tm_hour, tmi->tm_min, tmi->tm_sec);
	str = buff;
}
