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

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;


	/*
	//std::string hik_volume_name = "\\\\.\\physicaldrive0";
	std::string hik_volume_name = "F:\\41045\\2016-11-15--16-27-04-=-2016-11-15--20-01-00--[752773096760]-4.h264";
	HIKV::HikVolume vol(hik_volume_name);
	W32Lib::FileEx out_file("F:\\41045\\frames_0xE0.txt");
	if (vol.Open() && out_file.Create()) {
		
		HIKV::Frame frame;
		frame.data.reserve(0xffff);

		//vol.SetPointer(156400LL * 512);
		while(vol.FindNextFrame() != -1) {
			while(vol.ReadFrame(frame)) {

				std::string frame_info = "";

				std::vector<char> buff(16, 0x00);

				sprintf_s(&buff[0], buff.size(), "%011llX", frame.offset);

				std::string offset_str;
				offset_str += "[";
				offset_str += buff.data();
				offset_str += "] ";

				frame_info += offset_str;

				if (frame.data.size() == 0) {
					continue;
				}

				sprintf_s(&buff[0], buff.size(), "0x%02X", frame.Type());

				frame_info += "type: ";
				frame_info += buff.data();
				frame_info += "; ";

				size_t pos = 0;
				size_t count = 0;


				if (frame.Type() == 0xBA) {
					pos = 4;
				} else {
					pos = 6;
				}

				if (frame.Type() == 0xBC) {

					dvr::Timestamp stamp = frame.TimeStamp();

					continue;
				}

				if (frame.Type() != 0xE0) {
					continue;
				}


				HIKV::FRAME_TYPE_0E *frm = (HIKV::FRAME_TYPE_0E *)&frame.data[0];

				size_t tail = frame.data.size() - pos;

				count = tail < 32 ? tail : 32;

				assert(pos + count <= frame.data.size());

				frame_info += "data: [";

				ToHexString(&frame.data[pos], count, frame_info);

				frame_info += " ]";

				if (frame.Type() == 0xBA) {
					frame_info += " ####### ";
				}

				frame_info += "\n";

				out_file.Write(&frame_info[0], frame_info.length());
			
			}
		}
	}
	*/


	w.show();
	return a.exec();
}
