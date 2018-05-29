#include "1CDfile.h"

void File1CD::ReadPage(uint32_t page_num, std::vector<uint8_t> &page)
{
	if (!io.SetPointer(page_num * page_size)) {
		std::system_error sys_err(::GetLastError(), std::system_category());
	}
	if (io.Read(&page[0], page.size()) < page.size()) {
		std::system_error sys_err(::GetLastError(), std::system_category());
	}
}

bool File1CD::Open(void)
{
	if (!io.Open()) {
		return false;
	}

	return true; 
}

bool File1CD::GetObject(uint32_t page_num, DbObject & obj)
{
	std::vector<uint8_t> page(page_size);
	ReadPage(page_num, page);

	ObjectHeader *hdr = (ObjectHeader *)page.data();

	if (std::memcmp(hdr->signature, "1CDBOBV8", 8) != 0) {
		return false;
	}

	obj.header = *hdr;
	obj.hdr_page = page_num;
	obj.object_size = obj.header.object_size;

	if (obj.object_size) {

		size_t total_pages = (obj.object_size + (page_size - 1)) / page_size;
		uint32_t max_page_num = (uint32_t)(io.GetSize() / page_size);
		

		size_t pages_count = 0;
		uint32_t *alloc_page_num = hdr->blocks;
		while (*alloc_page_num) {
		
			ReadPage(*alloc_page_num, page);
		
		}
	}


	return true;
}

/*
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

		while (remained) {

			std::memset(&block[0], 0x00, block.size());

			BlobBlockHeader *hdr = (BlobBlockHeader *)block.data();

			if (remained >= sizeof(BlobBlockHeader::data)) {
				hdr->next_block = next_block;
				hdr->data_size = sizeof(BlobBlockHeader::data);
			}
			else {
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
*/


