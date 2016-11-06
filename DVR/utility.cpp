#include "utility.h"
#include <set>
#include <cassert>


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

void Convert2Avi(const std::string &raw_file_name, const std::string &avi_file_name)
{
	std::string cmd_line = "ffmpeg -f h264 -i ";
	cmd_line += raw_file_name;
	
	// -an - without audio 
	//cmd_line += " -an -vcodec copy ";
	
	cmd_line += " -vcodec copy ";
	cmd_line += avi_file_name;

	system(cmd_line.data());
}

bool FindByteString(const std::vector<BYTE>& buffer, size_t start_pos, const std::vector<BYTE>& string, size_t &find_pos)
{
	assert(buffer.size());
	assert(string.size());

	size_t pos = start_pos;

	std::set<BYTE> bytes;
	for (auto b : string) {
		auto it = bytes.insert(b);
	}

	while ((pos + string.size()) <= buffer.size()) {
		if (bytes.end() != bytes.find(buffer[pos + string.size() - 1])) {
			for (int i = 0; i < string.size(); ++i) {
				if (0x00 == std::memcmp(&buffer[pos], string.data(), string.size())) {
					find_pos = pos;
					return true;
				}
				pos++;
			}	
		} else {
			pos += string.size();
		}
	}

	return false;
}

bool FindByteStringBruteforce(const std::vector<BYTE>& buffer, size_t start_pos, const std::vector<BYTE>& string, size_t & find_pos)
{
	assert(buffer.size());
	assert(string.size());

	size_t pos = start_pos;

	while ((pos + string.size()) <= buffer.size()) {
		for (int i = 0; i < string.size(); i++) {		
			if (buffer[pos + i] != string[i]) {
				break;
			} else {
				if (i == (string.size() - 1)) {
					find_pos = pos;
					return true;
				}
			}		
		}
		pos++;
	}

	return false;
}
