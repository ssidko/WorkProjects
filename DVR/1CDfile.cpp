#include "1CDfile.h"

using namespace db1cd;

void DbFile::ReadPage(uint32_t page_num, std::vector<uint8_t> &buffer)
{
	if (buffer.size() != page_size) {
		buffer.resize(page_size);
	}
	if (!io.SetPointer(page_num * page_size)) {
		std::system_error sys_err(::GetLastError(), std::system_category());
	}
	if (io.Read(&buffer[0], buffer.size()) < buffer.size()) {
		std::system_error sys_err(::GetLastError(), std::system_category());
	}
}

DbFile::DbFile(const std::string & db_file_name, db1cd::PageSize size) :
	io(db_file_name.c_str()),
	page_size((uint32_t)size)
{
	max_pages_per_alloc_page = (page_size - (sizeof(AllocationPage) - sizeof(AllocationPage::pages[0]))) / sizeof(AllocationPage::pages[0]);
	max_allocation_page_count = (page_size - (sizeof(ObjectHeader) - sizeof(ObjectHeader::alloc_pages[0]))) / sizeof(ObjectHeader::alloc_pages[0]);
	max_object_size = max_allocation_page_count * max_pages_per_alloc_page * page_size;
}

bool DbFile::Open(void)
{
	if (!io.Open()) {
		return false;
	}

	max_page_num = io.GetSize() / page_size;

	return true; 
}

std::shared_ptr<Object> db1cd::DbFile::GetObject(uint32_t page_num)
{
	std::vector<uint8_t> page(page_size);
	ReadPage(page_num, page);

	ObjectHeader *hdr = (ObjectHeader *)page.data();

	if (std::memcmp(hdr->signature, "1CDBOBV8", 8) != 0) {
		return std::shared_ptr<Object>();
	}

	std::shared_ptr<Object> obj = std::make_shared<Object>(page_num, *hdr, *this);

	if (obj->Size()) {
		uint32_t total_pages = ((*obj).Size() + (page_size - 1)) / page_size;
		uint32_t alloc_pages_count = (total_pages + (max_pages_per_alloc_page - 1)) / max_pages_per_alloc_page;

		if (obj->Size() > max_object_size) {
			throw std::exception();
		}

		uint32_t *pos = hdr->alloc_pages;
		obj->alloc_pages.resize(alloc_pages_count);
		for (auto & alloc_page : obj->alloc_pages) {
			alloc_page = *pos++;
		}
	}

	return obj;
}

void db1cd::Object::ReadAllocationTable(void)
{
	if (!allocation_table) {
		uint32_t total_pages = (header.object_size + (db.PageSize() - 1)) / db.PageSize();
		std::vector<uint32_t> table(total_pages);
		
		size_t pos = 0;
		std::vector<uint8_t> alloc_page_buff(db.PageSize());
		AllocationPage *alloc_page = (AllocationPage *)&alloc_page_buff[0];
		for (auto alloc_page_num : alloc_pages) {
		
			db.ReadPage(alloc_page_num, alloc_page_buff);
			if (alloc_page->pages_count > db.max_pages_per_alloc_page) {
				throw std::exception();
			}
		
			for(int i = 0; i < alloc_page->pages_count; i++) {
			
				table[pos++] = alloc_page->pages[i++];
			
			}
		
		}


	}
}

void db1cd::Object::Read(std::vector<uint8_t> &buffer)
{
	uint32_t to_read = header.object_size;
	uint32_t readed = 0;

	if (buffer.size() != header.object_size) {
		buffer.resize(header.object_size);
	}

	uint8_t *pos = &buffer[0];
	std::vector<uint8_t> buff(db.PageSize());
	AllocationPage *alloc_page = (AllocationPage *)&buff[0];

	for (const auto & alloc_page_num : alloc_pages) {
		
		db.ReadPage(alloc_page_num, buff);
		if (alloc_page->pages_count > db.max_pages_per_alloc_page) {
			throw std::exception();
		}

		uint32_t page_num = 0;
		for (int i = 0; i < alloc_page->pages_count; i++) {
			page_num = alloc_page->pages[i];
		}
	
	}

}

void db1cd::Object::SaveToFile(const std::string &file_path)
{
	W32Lib::FileEx file(file_path.c_str());
	if (file.Create()) {
	
		size_t size = header.object_size;
		size_t page_size = db.PageSize();
		std::vector<uint8_t> alloc_page_buff(db.PageSize());
		std::vector<uint8_t> data_page_buff(db.PageSize());
		AllocationPage *alloc_page = (AllocationPage *)alloc_page_buff.data();

		uint32_t *alloc_page_num = header.alloc_pages;

		while (*alloc_page_num && size) {
		
			db.ReadPage(*alloc_page_num, alloc_page_buff);
			for (size_t i = 0; i < alloc_page->pages_count; i++) {
			
				db.ReadPage(alloc_page->pages[i], data_page_buff);

				uint32_t to_write = size >= page_size ? page_size : size;
				if (to_write) {
					file.Write(data_page_buff.data(), to_write);
					size -= to_write;
				}
			}	
		
		}	
	}
}


bool IsValidDbObject(W32Lib::FileEx &io, ObjectHeader &obj_header, uint32_t page_size, uint32_t max_block_num)
{
	size_t total_blocks = 0;
	size_t blk_idx = 0;
	uint32_t first_page = 0;
	std::vector<uint8_t> buff(page_size);
	if (obj_header.object_size && obj_header.alloc_pages[0]) {
		while (obj_header.alloc_pages[blk_idx]) {

			io.SetPointer(page_size * obj_header.alloc_pages[blk_idx]);
			if (io.Read(buff.data(), page_size) != page_size) {
				return false;
			}

			AllocationPage *table = (AllocationPage *)buff.data();

			if ((table->pages_count < 1) || (table->pages_count > sizeof(AllocationPage::pages))) {
				return false;
			}

			int curr_blocks = 0;
			for (int i = 0; i < sizeof(AllocationPage::pages); i++) {
				if ((table->pages[i] == 0) || (table->pages[i] > max_block_num)) {
					break;
				}
				if (blk_idx == 0 && i == 0) {
					first_page = table->pages[i];
				}
				curr_blocks++;
				total_blocks++;
			}

			if (curr_blocks != table->pages_count) {
				return false;
			}

			blk_idx++;
		}

		if (((obj_header.object_size + (page_size - 1)) / page_size) != total_blocks) {
			return false;
		}

		io.SetPointer(first_page * page_size);
		if (io.Read(buff.data(), page_size) != page_size) {
			return false;
		}

		uint32_t *obj_sign = (uint32_t *)buff.data();
		if (*obj_sign == 0x0022007B) {
			return true;
		}
	}
	return false;
}

//void MakeBlob(void)
//{
//	W32Lib::FileEx in("F:\\44322\\12\\noname");
//	W32Lib::FileEx out("F:\\44322\\pages-2.bin");
//	if (in.Open() && out.Create()) {
//
//		uint32_t next_block = 2;
//		uint32_t remained = (uint32_t)in.GetSize();
//		std::vector<uint8_t> block(256);
//
//		while (remained) {
//
//			std::memset(&block[0], 0x00, block.size());
//
//			BlobBlockHeader *hdr = (BlobBlockHeader *)block.data();
//
//			if (remained >= sizeof(BlobBlockHeader::data)) {
//				hdr->next_block = next_block;
//				hdr->data_size = sizeof(BlobBlockHeader::data);
//			}
//			else {
//				hdr->next_block = 0x00;
//				hdr->data_size = remained;
//			}
//
//			in.Read(&hdr->data[0], hdr->data_size);
//			out.Write(block.data(), block.size());
//
//			next_block++;
//			remained -= hdr->data_size;
//
//		}
//	}
//}

void db1cd::RestoreRootObject()
{
	W32Lib::FileEx db("F:\\44552\\Repaired\\Козын-1\\1Cv8.1CD");
	W32Lib::FileEx root_object("F:\\44552\\Repaired\\Козын-1\\root_object.bin");
	if (db.Open() && root_object.Create()) {

		const size_t page_size = PageSize::size_4kb;
		const uint32_t max_page = db.GetSize() / page_size;

		uint32_t page = 0;
		size_t valid_objects = 0;
		std::vector<uint8_t> buffer(4096, 0);

		page = 3;
		db.SetPointer(page_size * page);
		while (db.Read(buffer.data(), buffer.size()) == buffer.size()) {
			if (std::memcmp(buffer.data(), "1CDBOBV8", 8) == 0) {

				ObjectHeader *obj_header = (ObjectHeader *)buffer.data();
				if (IsValidDbObject(db, *obj_header, page_size, max_page)) {
					root_object.Write(&page, sizeof(page));
					valid_objects++;
				}
			}
			page++;
			db.SetPointer(page_size * page);
		}
	}
}

