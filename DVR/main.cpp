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
#include "dvr.h"
#include "Hikvision.h"
#include "WFS.h"
#include "dcH264_Reader.h"
#include "G2fdbVolume.h"
#include "utility.h"
#include "G2fdbRecovery.h"

class WinApiException : public std::exception
{
private:
	DWORD error_code;
	std::string error_description;
public:
	WinApiException() : error_code(::GetLastError())
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
	uint8_t buff[] = { 0x67, 0x64, 0x00, 0x14, 0xAD, 0x84, 0x01, 0x0C, 0x20, 0x08, 0x61, 0x00, 0x43, 0x08, 0x02, 0x18, 0x40, 0x10, 0xC2, 0x00, 0x84, 0x2B, 0x50, 0xB0, 0x4B, 0x20 };
	bitstream_reader bs(buff, sizeof(buff));
	bitstream_reader bs2(buff, sizeof(buff));

	size_t bits_count = bs.bits_available();
	size_t size_sps = sizeof(sps);

	int forbidden_zero_bit = bs.f(1);
	int nal_ref_idc = bs.u(2);	
	int nal_unit_type = bs.u(5);

	bs2.f(1);
	bs2.u(2);
	bs2.u(5);

	size_t width = 0;
	size_t height = 0;

	if (nal_unit_type == 7) {
		ReadSPS(bs, sps);
		//GetResolution(bs2, width, height);
		h264_GetWidthHeight(bs2, width, height);
	}

	return;
}

#include "DhfsVolume.h"
#include "DhfsRecovery.h"
#include "GetWidthHeight.h"

void ToHexString(uint8_t *buff, size_t count, std::string &str)
{
	char ch[4] = { 0 };
	for (int i = 0; i < count; i++) {
		sprintf_s(ch, sizeof(ch), "%02X", buff[i]);
		str += " ";
		str += ch;
	}
}

#include "MyPrintf.h"
#include "zfs_test.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	//dcH264::main();
	//zfs_test();
	//zfs_dnode_recovery();
	
	w.show();
	return a.exec();
}
