#ifndef _1CDFILE_H
#define _1CDFILE_H

#include <stdint.h>
#include <string>
#include <vector>
#include "File.h"

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
	uint32_t pages_count;
	uint32_t pages[1023];
};

struct BlobHeader {
	uint32_t next_block;
	uint16_t data_size;
	uint8_t data[250];
};

#pragma pack(pop)

enum class DbPageSize {
	size_4kb = 4096,
	size_8kb = 8192,
};

struct DbObject {
	uint32_t hdr_page;
	ObjectHeader header;
	uint32_t object_size;
	std::vector<uint32_t> alloc_pages;
};

class File1CD
{
private:
	W32Lib::FileEx io;
	const uint32_t page_size;
	void ReadPage(uint32_t page_num, std::vector<uint8_t> &page);
public:
	File1CD(const std::string &db_file_name, DbPageSize size = DbPageSize::size_4kb) : io(db_file_name.c_str()), page_size((uint32_t)size) {}
	bool Open(void);
	bool GetObject(uint32_t page_num, DbObject &obj);
};

#endif // _1CDFILE_H
