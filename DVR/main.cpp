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

const uint32_t block_size = 4096;

#pragma pack(push,1)

struct ObjectHeader {
	char signature[8]; // "1CDBOBV8"
	uint32_t object_size;
	uint32_t version1;
	uint32_t version2;
	uint32_t version;
	uint32_t blocks[1];
};

struct AllocationTable {
	uint32_t numblocks;
	uint32_t blocks[1023];
};

struct BlobBlockHeader {
	uint32_t next_block;
	uint16_t data_size;
	uint8_t data[250];
};

#pragma pack(pop)

bool IsValidDbObject(W32Lib::FileEx &io, ObjectHeader &obj_header, uint32_t max_block_num)
{
	size_t total_blocks = 0;
	size_t blk_idx = 0;
	uint32_t first_block = 0;
	std::vector<uint8_t> buff(block_size);
	if (obj_header.object_size && obj_header.blocks[0]) {
		while (obj_header.blocks[blk_idx]) {
			
			io.SetPointer(block_size * obj_header.blocks[blk_idx]);
			if (io.Read(buff.data(), block_size) != block_size) {
				return false;
			}

			AllocationTable *table = (AllocationTable *)buff.data();

			if ((table->numblocks < 1) || (table->numblocks > sizeof(AllocationTable::blocks))) {
				return false;
			}

			int curr_blocks = 0;
			for (int i = 0; i < sizeof(AllocationTable::blocks); i++) {
				if ((table->blocks[i] == 0) || (table->blocks[i] > max_block_num)) {
					break;
				}
				if (blk_idx == 0 && i == 0) {
					first_block = table->blocks[i];
				}
				curr_blocks++;
				total_blocks++;
			}

			if (curr_blocks != table->numblocks) {
				return false;
			}

			blk_idx++;
		}

		if (((obj_header.object_size + (block_size - 1)) / block_size) != total_blocks) {
			return false;
		}

		io.SetPointer(first_block * block_size);
		if (io.Read(buff.data(), block_size) != block_size) {
			return false;
		}

		uint32_t *obj_sign = (uint32_t *)buff.data();
		if (*obj_sign == 0x0022007B) {
			return true;
		}	
	}
	return false;
}

void MakeBlob(void)
{
	W32Lib::FileEx in("F:\\44322\\12\\noname");
	W32Lib::FileEx out("F:\\44322\\pages-2.bin");
	if (in.Open() && out.Create()) {

		uint32_t next_block = 2;
		uint32_t remained = (uint32_t)in.GetSize();
		std::vector<uint8_t> block(256);

		while(remained) {
		
			std::memset(&block[0], 0x00, block.size());

			BlobBlockHeader *hdr = (BlobBlockHeader *)block.data();

			if (remained >= sizeof(BlobBlockHeader::data)) {
				hdr->next_block = next_block;
				hdr->data_size = sizeof(BlobBlockHeader::data);
			} else {
				hdr->next_block = 0x00;
				hdr->data_size = remained;
			}

			in.Read(&hdr->data[0], hdr->data_size);
			out.Write(block.data(), block.size());

			next_block++;
			remained -= hdr->data_size;		
		
		}
	}
}

void Test1C8()
{
	W32Lib::FileEx db("F:\\44322\\1Cv8-2.1CD");
	//W32Lib::FileEx db("F:\\44322\\12\\1Cv8.1CD");
	W32Lib::FileEx pages("F:\\44322\\pages-2.bin");
	if (db.Open() && pages.Create()) {

		uint32_t block = 0;
		size_t valid_objects = 0;
		std::vector<uint8_t> buffer(4096, 0);

		block = 3;
		db.SetPointer(block_size * block);
		while (db.Read(buffer.data(), buffer.size()) == buffer.size()) {
			if (std::memcmp(buffer.data(), "1CDBOBV8", 8) == 0) {

				ObjectHeader *obj_header = (ObjectHeader *)buffer.data();
				if (IsValidDbObject(db, *obj_header, 285893)) {
					pages.Write(&block, sizeof(block));
					valid_objects++;
				}
			}
			block++;
			db.SetPointer(block_size * block);
		}
	}
}


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	//Test1C8();
	MakeBlob();

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
