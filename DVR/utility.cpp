#include "utility.h"


WORD BeToLe(WORD word)
{
	WORD res;
	BYTE *be = (BYTE *)&word;
	BYTE *le = (BYTE *)&res;
	le[0] = be[1];
	le[1] = be[0];
	return res;
}

DWORD BeToLe(DWORD dword)
{
	DWORD res;
	BYTE *be = (BYTE *)&dword;
	BYTE *le = (BYTE *)&res;
	le[0] = be[3];
	le[1] = be[2];
	le[2] = be[1];
	le[3] = be[0];
	return res;
}

void ConvertToMkv(std::string &raw_file_name, std::string &mkv_file_name)
{
	std::string convertor_app("D:\\Soft\\#RecoverySoft#\\mkvtoolnix\\mkvmerge.exe ");

	std::string cmd_line;
	cmd_line += "mkvmerge -o ";
	cmd_line += mkv_file_name;
	cmd_line += " ";
	cmd_line += raw_file_name;

	system(cmd_line.data());
}

void Convert2Avi(std::string &raw_file_name, std::string &avi_file_name)
{
	std::string cmd_line = "ffmpeg -f h264 -i ";
	cmd_line += raw_file_name;
	cmd_line += " -an -vcodec copy ";
	cmd_line += avi_file_name;

	system(cmd_line.data());
}
