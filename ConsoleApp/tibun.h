#include "stdafx.h"
#include "zlib.h"

//#define MARK_SIZE						8
#define MARK_SIZE						4
#define UNPACK_DATA_SIZE				(DWORD)2048*512

int Unpack(FileEx *src_file, FileEx *out_file);
int tibun_main(int argc, _TCHAR* argv[]);
