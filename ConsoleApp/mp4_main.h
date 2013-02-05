#pragma once
#include "W32Lib.h"
#include <assert.h>
#include "Mp4File.h"

#define CLUSTER_SIZE		512*64

#define MP4_FRAMES_OFFSET								(DWORD)0x0001682C

int mp4_main(TCHAR *in_file, TCHAR *out_dir);
