#include "stdafx.h"
#include "iFile.h"
#include "WinFile.h"

int _tmain(int argc, _TCHAR* argv[])
{
	MyLib::WinFile file(_T("test.bin"));

	file.Create(MyLib::FileMode::kReadWrite);

	return 0;
}