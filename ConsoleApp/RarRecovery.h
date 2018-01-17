#ifndef _RAR_RECOVERY_H
#define _RAR_RECOVERY_H

#include <windows.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include "rar.h"

#define CMD_ARG_SOURCE_FILE			1
#define CMD_ARG_OUT_FILE			2

int RarRecoveryMain(int argc, _TCHAR* argv[]);
int RarRecovery(const std::string &arch_path, const std::string &out_arch_path);
void ExamineFile(const std::string &arch_path);

#endif // _RAR_RECOVERY_H

