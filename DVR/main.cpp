#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QDir>

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

#include <iostream>

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

#include <iostream>
#include "bitstream_reader.h"
#include "h264_ps.h"

void h264_test(void)
{
	SPS sps = { 0 };
	uint8_t buff[] = { /*0x00, 0x00, 0x00, 0x01,*/ 0x61, 0xe0, 0xc0, 0x1E, 0x95, 0xA8, 0x2C, 0x04, 0x99 };
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

	h264_test();

	w.show();
	return a.exec();
}
