#ifndef _1CDFILE_H
#define _1CDFILE_H

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "File.h"

namespace db1cd
{

#pragma pack(push,1)

	struct ObjectHeader {
		char signature[8]; // "1CDBOBV8"
		uint32_t object_size;
		uint32_t flag1;
		uint32_t flag2;
		uint32_t flag3;
		uint32_t alloc_pages[1];
	};

	struct AllocationPage{
		uint32_t pages_count;
		uint32_t pages[1];
	};

	struct BlobHeader {
		uint32_t next_block;
		uint16_t data_size;
		uint8_t data[250];
	};

#pragma pack(pop)

	enum class PageSize {
		size_4kb = 4096,
		size_8kb = 8192,
	};

	class DbFile;

	class Object
	{
		friend DbFile;
	private:
		DbFile &db;
		uint32_t page;
		ObjectHeader header;
		std::vector<uint32_t> alloc_pages;
		std::optional<std::vector<uint32_t>> allocation_table;
	public:
		Object(uint32_t obj_page, ObjectHeader &obj_header, DbFile &data_base) : page(obj_page), header(obj_header), db(data_base) {}
		void ReadAllocationTable(void);
		void Read(std::vector<uint8_t> &data_buff);
		//void Write();
		uint32_t Size(void) { return header.object_size; }
	};

	class Table
	{
	private:
	public:
	};

	class DbFile
	{
		friend Object;
	public:
		DbFile(const std::string &db_file_name, PageSize size);
		bool Open(void);
		uint32_t PageSize(void) { return page_size; }
		std::shared_ptr<Object> GetObject(uint32_t page_num);

	private:
		W32Lib::FileEx io;
		const uint32_t page_size;
		uint32_t max_page_num;

		uint32_t max_pages_per_alloc_page;
		uint32_t max_allocation_page_count;
		size_t max_object_size;

		void ReadPage(uint32_t page_num, std::vector<uint8_t> &page);
	};
}



#endif // _1CDFILE_H
