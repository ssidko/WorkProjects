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
#include "1CDfile.h"
#include "ExtentRecovery.h"

#include <filesystem>


namespace fs = std::experimental::filesystem;

void ttt()
{
	using namespace ext4;

	std::map<uint32_t, std::string> files;
	fs::path extent_dir("F:\\44326\\extents");
	for (auto &it : fs::directory_iterator(extent_dir)) {
		std::string file_name = it.path().filename().string();
		uint32_t block = std::stoi(file_name);
		files[block] = it.path().string();
	}

	uint32_t block = 1861255175;
	fs::path out_dir("F:\\44326\\file");
	fs::create_directories(out_dir);

	auto it = files.find(block);
	if (it != files.end()) {

		W32Lib::FileEx io(it->second.c_str());
		if (io.Open()) {
			
			std::vector<uint8_t> buff(io.GetSize());
			EXTENT_BLOCK *extent_block = (EXTENT_BLOCK *)&buff[0];

			io.Read(&buff[0], buff.size());
			io.Close();

			assert(extent_block->header.depth);

			fs::rename(it->second, out_dir/fs::path(it->second).filename());

			for (int i = 0; i < extent_block->header.entries; i++) {
				auto itt = files.find((uint32_t)extent_block->extent_index[i].PysicalBlock());
				if (itt != files.end()) {
					fs::rename(itt->second, out_dir / fs::path(itt->second).filename());
				}
			}
		
		}	
	}

}

bool SaveAllBlocksWithExtents(const std::string &io_file_name, const std::string &out_dir_path)
{
	using namespace ext4;

	const size_t block_size = 4096;
	const size_t blocks_count = 2196904704;
	const size_t extents_per_block = (block_size - sizeof(EXTENT))/sizeof(EXTENT);

	W32Lib::FileEx io(io_file_name.c_str());
	if(!io.Open()) {
		return false;
	}

	size_t block = 0;
	std::vector<uint8_t> buff(block_size);
	char str_buff[255] = { 0 };

	for (size_t block = 0; block < blocks_count; block++) {
			
		io.SetPointer(block * block_size);
		io.Read(&buff[0], block_size);

		EXTENT_BLOCK *extent_block = (EXTENT_BLOCK *)&buff[0];

		if (extent_block->header.magic == EXTENT_HEADER_MAGIC) {
			if (extent_block->header.max == extents_per_block) {
				if (extent_block->header.entries) {
				
					std::sprintf(str_buff, "%010lld level=%d", block, extent_block->header.depth);
					std::string file_name(str_buff);
					std::string out_file_path = out_dir_path + file_name;

					W32Lib::FileEx out_file(out_file_path.c_str());
					if (!out_file.Create()) {
						throw std::exception();
					}

					out_file.Write(buff.data(), (extent_block->header.entries + 1) * sizeof(EXTENT));
				
				}
			}
		}	
	}
	return true;
}

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	//SaveAllBlocksWithExtents("F:\\44326\\raid5.img", "F:\\44326\\extents\\");

	ttt();

	using namespace db1cd;

	DbFile db("F:\\44322\\examples\\1Cv8.1CD", PageSize::size_4kb);
	if (db.Open()) {
	
		std::vector<uint8_t> buff;

		std::shared_ptr<Object> obj = db.GetObject(16);
		if (obj) {		
			auto obj_size = obj->Size();
			obj->Read(buff);

		}

		std::shared_ptr<Object> obj2 = db.GetObject(17736);
	
	}

	//Test1C8();
	//MakeBlob();

	//dcH264::main();
	//zfs_test();
	//zfs_dnode_recovery();

	//G2FDB::TestRecovery();
	//G2FDB::RcoveryByMetadata();

	//std::system_error sys_err(ERROR_ALREADY_EXISTS, std::system_category());	
	//std::string what = sys_err.what();

	w.show();
	return a.exec();
}
