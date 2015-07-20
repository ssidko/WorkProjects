#include "DvrExt3.h"


bool dvrext::IsIndirectBlock(const EXT_BLOCK<EXT_BLOCK_SIZE> &block)
{
	DWORD max_valid_pointer_index = 0xFFFFFFFF;
	DWORD pointers_per_block = block.DwordsCount();

	/*
	// 
	// ”казатели на блоки не должны указывать за пределы раздела.
	// ≈сли такие встречаютс€ - блок не €вл€етс€ IndirectBlock-ом
	// 
	// ¬нутри IndirectBlcok-а не может быть два одинаковых указател€,
	// за исключением нулевого указател€.
	//
	*/

	std::set<DWORD> set;
	std::set<DWORD>::iterator it;
	std::pair<std::set<DWORD>::iterator, bool> ret;
	for (DWORD i = 0; i < pointers_per_block; ++i) {
		if (block.dword[i] <= EXT_MAX_BLOCK_NUMBER) {
			if (block.dword[i]) {
				ret = set.insert(block.dword[i]);
				if (ret.second == false) {
					if (*ret.first == block.dword[i]) {
						return false;
					}
				}
			}
		} else {
			return false;
		}
	}

	return true;
}

int dvrext::SaveAllIndirectBlocksToFolder(std::string out_folder, std::string volume_file_name, const LONGLONG &offset, const DWORD &blocks_count, DWORD block_size)
{


	return 0;
}

//
// volume_offset;
// min_valid_block_number;
// max_valid_block_number;
// start_block;
// blocks_count;
//
 

int dvrext::Run()
{
	std::string out_dir = "F:\\38023\\iblocks\\";
	std::string out_file_name = "";

	EXT_BLOCK<EXT_BLOCK_SIZE> block;
	block.Erase();

	LONGLONG file_couner = 0;
	LONGLONG start_block = 27792465;
	LONGLONG block_number = start_block;
	BufferedFile ext_volume("\\\\.\\PhysicalDrive2");
	if (ext_volume.Open()) {
		ext_volume.SetPointer(EXT_VOLUME_OFFSET + start_block * EXT_BLOCK_SIZE);
		for (; block_number <= EXT_MAX_BLOCK_NUMBER; ++block_number) {
			if (ext_volume.Read(block.raw, block.Size()) == block.Size()) {
				if (IsIndirectBlock(block)) {
					out_file_name = out_dir + std::to_string(block_number);
					W32Lib::FileEx file(out_file_name.c_str());
					if (file.Create()) {
						file.Write(block.raw, block.Size());
						++file_couner;
					}
				}			
			} else {
				return -1;
			}
		}	
	}

	return 0;
}
