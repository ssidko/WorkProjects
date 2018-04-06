#ifndef _UTILITY_H
#define _UTILITY_H

#include <string>
#include <vector>
#include <functional>
#include <windows.h>

WORD BeToLe(WORD word);
DWORD BeToLe(DWORD dword);

void ConvertToMkv(const std::string &raw_file_name, const std::string &mkv_file_name);
void Convert2Avi(const std::string &raw_file_name,  const std::string &avi_file_name);

bool FindByteString(const std::vector<BYTE>& buffer, size_t start_pos, const std::vector<BYTE>& string, size_t &find_pos);
bool FindByteStringBruteforce(const std::vector<BYTE>& buffer, size_t start_pos, const std::vector<BYTE>& string, size_t &find_pos);

void EnumeratePhysicalDrives(std::function<void(const std::string &)> call_back);

bool CurrentDirectory(std::string &current_directory);

void CreateDir(const std::string &dir_path);

LONGLONG GetPhysicalDriveSize(const std::string &name);
LONGLONG FileSize(const std::string &file_name);

std::string string_format(const std::string fmt_str, ...);
std::string format_string(const std::string fmt_str, ...);

#endif // _UTILITY_H

