#include "utility.h"
#include <set>
#include <cassert>
#include "File.h"

#include <stdarg.h> 
#include <memory>


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

void ConvertToMkv(const std::string &raw_file_name, const std::string &mkv_file_name)
{
	//std::string convertor_app("D:\\Soft\\#RecoverySoft#\\mkvtoolnix\\mkvmerge.exe ");
	//std::string convertor_app = ".\\bin\\mkvmerge.exe ";

	std::string cmd_line;
//	cmd_line += "mkvmerge -o ";
	cmd_line += ".\\bin\\mkvmerge.exe -o ";
	cmd_line += mkv_file_name;
	cmd_line += " ";
	cmd_line += raw_file_name;

	system(cmd_line.data());
}

void Convert2Avi(const std::string &raw_file_name, const std::string &avi_file_name)
{
	//std::string cmd_line = "ffmpeg -f h264 -i ";
	//D:\GitHub\WorkProjects\DVR\ffm\bin\ffmpeg.exe

	std::string cmd_line;
	cmd_line += ".\\bin\\ffmpeg.exe -f h264 -i ";
	cmd_line += "\"";
	cmd_line += raw_file_name;
	cmd_line += "\"";
	
	// -an - without audio 
	//cmd_line += " -an -vcodec copy ";
	
	cmd_line += " -vcodec copy ";
	cmd_line += "\"";
	cmd_line += avi_file_name;
	cmd_line += "\"";

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

void EnumeratePhysicalDrives(std::function<void(const std::string &)> call_back)
{
	std::string prefix = "\\\\.\\PhysicalDrive";
	for (size_t i = 0; i < 32; i++) {	
		std::string drive_name = prefix + std::to_string(i);
		W32Lib::FileEx drive(drive_name.c_str());
		if (drive.Open()) {
			call_back(drive_name);
		}	
	}
}

bool CurrentDirectory(std::string &current_directory)
{
	current_directory = "";
	std::vector<char> buff;

	DWORD size = ::GetCurrentDirectoryA(0, nullptr);
	if (size) {
		buff.resize(size + 1, 0x00);
		if (size = ::GetCurrentDirectoryA(buff.size(), &buff[0])) {
			current_directory = buff.data();
			return true;
		}
	} else {
		DWORD error = ::GetLastError();
		int x = 0;
	}
	return false;
}

LONGLONG GetPhysicalDriveSize(const std::string & name)
{
	bool result = false;
	DWORD rw;
	DISK_GEOMETRY_EX geometry = {0};

	W32Lib::FileEx drive(name.data());
	if (drive.Open()) {

		result = DeviceIoControl(drive.Handle(),
								IOCTL_DISK_GET_DRIVE_GEOMETRY_EX,
								NULL, 0,
								&geometry, sizeof(DISK_GEOMETRY_EX),
								&rw, NULL);

		if (result) {
			return geometry.DiskSize.QuadPart;
		}
	}

	return -1;
}

std::string string_format(const std::string fmt_str, ...) {
	int final_n, n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */
	std::string str;
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1) {
		formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
		strcpy(&formatted[0], fmt_str.c_str());
		va_start(ap, fmt_str);
		final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::string(formatted.get());
}
