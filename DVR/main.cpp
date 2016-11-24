#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QDir>

#include <iostream>
#include <vector>
#include "File.h"
#include "BufferedFile.h"

#include "WinConsole.h"
#include "TestWidget.h"
#include "Raid5.h"
#include "Orbita.h"

inline void _trace(char *format, ...)
{
	static char buffer[2048] = { 0 };
	DWORD rw = 0;

	va_list argptr;
	va_start(argptr, format);
	vsprintf_s(buffer, format, argptr);
	va_end(argptr);

	::WriteConsoleA(::GetStdHandle(STD_OUTPUT_HANDLE), buffer, strlen(buffer), &rw, NULL);
}

#include "ExtentRecovery.h"
#include "FileRecordRecovery.h"
#include "Timestamp.h"
#include "Hikvision.h"
#include "dhfs.h"
#include "WFS.h"
#include "G2fdbVolume.h"
#include "utility.h"
#include "G2fdbRecovery.h"

class WinApiException : public std::exception
{
private:
	DWORD error_code;
	std::string error_description;
public:
	WinApiException(DWORD win_error_code) : error_code(win_error_code)
	{
		if (error_code) {
			char *str = NULL;
			if (::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error_code, 0, (LPSTR)&str, 0, NULL)) {
				error_description = str;
				::LocalFree(str);
			} else {
				error_description = "Не удалось получить описание ошибки.";
			}
		}
	}

	virtual const char * what() const noexcept override
	{
		return error_description.c_str();
	}
};

#include "bitstream_reader.h"
#include "h264_ps.h"

void h264_test(void)
{
	SPS sps = { 0 };
	uint8_t buff[] = { 0x67, 0x64, 0x00, 0x28, 0xAC, 0xE8, 0x0E, 0x81, 0x26, 0xC0, 0x44};
	bitstream_reader bs(buff, sizeof(buff));

	size_t bits_count = bs.bits_available();
	size_t size_sps = sizeof(sps);

	int forbidden_zero_bit = bs.f(1);
	int nal_ref_idc = bs.u(2);	
	int nal_unit_type = bs.u(5);

	if (nal_unit_type == 7) {
		ReadSPS(bs, sps);
	}

	int x = 0;

	return;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	
	size_t counter = 0;
	LONGLONG offset = 0;
	uint8_t sign[] = { 0x44, 0x48, 0x41, 0x56 };
	BufferedFile io("\\\\.\\PhysicalDrive0", 128*512, 7020264960LL);
	if (io.Open()) {
		
		while (io.Find(sign, sizeof(sign), offset)) {
			io.SetPointer(offset + 1);
			counter++;		
		}	
		int x = 0;	
	}


	//counter = 0;
	//offset = 0;
	//W32Lib::FileEx file("F:\\40774\\find-test.bin");
	//if (file.Open()) {
	//	counter = 0;
	//	offset = 0;

	//	while ( -1 != (offset = file.Find(sign, sizeof(sign))) ) {
	//		file.SetPointer(offset + 1);
	//		counter++;		
	//	}	
	//}

	int x = 0;



	w.show();
	return a.exec();
}
