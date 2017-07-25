#ifndef _RAR_RECOVERY_H
#define _RAR_RECOVERY_H


#include "rar.h"
#include <string>

//#define ARCH_APP_PATH			".\7z.exe"
#define ARCH_APP_PATH			" D:\\GitHub\\7z\\7z.exe"

int RarRecovery(const std::string &arch_path, const std::string &out_arch_path);

#endif // _RAR_RECOVERY_H

