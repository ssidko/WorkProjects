#pragma once

#include "W32Lib.h"
#include <windows.h>
#include <assert.h>

namespace h264dvr
{

int main(TCHAR *file_name, LONGLONG offset, TCHAR *out_dir);

}
