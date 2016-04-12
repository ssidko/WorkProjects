#ifndef _UTILITY_H
#define _UTILITY_H

#include <string>
#include <windows.h>

WORD BeToLe(WORD word);
DWORD BeToLe(DWORD dword);

void Convert2Avi(const std::string &raw_file_name,  const std::string &avi_file_name);

#endif // _UTILITY_H

