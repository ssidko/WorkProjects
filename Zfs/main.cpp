#include "File.h"
#include "MException.h"

void GetErrorString(DWORD error_code, String &string)
{
	TCHAR *str = NULL;
	DWORD chars_number = 0;
	string.clear();
	chars_number = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, 0, (LPTSTR)&str, 0, NULL);
	if (chars_number) {
		string = str;
		::LocalFree(str);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	DWORD rw = 0;
	String file_name(_T("D:\\test.bin"));
	BYTE *buffer = new BYTE[100];
	try {
		File file(file_name, FileMode::kReadOnly);
		file.Open();

		file.SetPointer(100000ULL);
		rw = file.Write(buffer, 100);

		WinError error;

		file.SetPointer(0);
		rw = file.Read(buffer, 100);

		ULONGLONG size = 0;
		file.Size(size);

		throw 0;
	}
	catch (FileException const & exc) {
		const char * msg = exc.what();
		int x = 0;
	}
	
	if (buffer) {
		delete[] buffer;
		buffer = nullptr;
	}
	return 0;
}

