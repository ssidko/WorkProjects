#ifndef _UTILITY_H
#define _UTILITY_H

#include <string>
#include <vector>
#include <windows.h>

WORD BeToLe(WORD word);
DWORD BeToLe(DWORD dword);

void Convert2Avi(const std::string &raw_file_name,  const std::string &avi_file_name);

bool FindByteString(const std::vector<BYTE>& buffer, size_t start_pos, const std::vector<BYTE>& string, size_t &find_pos);
bool FindByteStringBruteforce(const std::vector<BYTE>& buffer, size_t start_pos, const std::vector<BYTE>& string, size_t &find_pos);

#endif // _UTILITY_H

