#include "RarRecovery.h"
#include "crc32.h"
#include "W32Lib.h"
#include <vector>

#define RAR_FILE_NAME_MAX_SIZE			2048 
#define RAR_FILE_HEADER_MAX_SIZE		sizeof(rar::FILE_HEADER) + RAR_FILE_NAME_MAX_SIZE + 128

bool IsValidFileHeaderCrc(rar::FILE_HEADER *header)
{
	uint32_t crc = crc32(&header->type, header->header_size - 2);
	if ((uint16_t)crc == header->crc) {
		return true;
	}
	return false;
}

bool IsValidFileHeader(rar::FILE_HEADER *header)
{
	if (header->type != rar::BlockType::FileHeader) {
		return false;
	}

	if (header->header_size > RAR_FILE_HEADER_MAX_SIZE) {
		return false;
	}

	if (header->name_size > RAR_FILE_NAME_MAX_SIZE) {
		return false;
	}

	if (header->os > rar::OperatingSystem::BeOS) {	
		return false;
	}

	size_t pack = header->packing_mathod;
	if ((pack < rar::PackingMethod::Storing) || (pack > rar::PackingMethod::BestCompression)) {
		return false;
	}

	// 0x100
	// HIGH_PACK_SIZE and HIGH_UNP_SIZE fields are present.
	// These fields are used to archive only very large files (larger than 2Gb).

	if ((header->flags & ~0x000100u) == 0) {
		if (header->uncompressed_size) {
			if (header->uncompressed_size > 2 * 1024 * 1024 * 1024U) {
				return false;
			}
			if (header->compressed_size > header->uncompressed_size) {
				return false;
			}
		} else {
			if (header->compressed_size != 0) {
				return false;
			}
		}
	}

	return true;
}

int64_t FindFileHeader(W32Lib::FileEx &file, int64_t start_offset = 0)
{
	std::vector<uint8_t> buff(128*1024, 0);
	rar::FILE_HEADER *header = nullptr;
	int64_t file_size = file.GetSize();
	int64_t offset = start_offset;
	size_t readed = 0;

	if (!file.SetPointer(offset)) {
		return -1;
	}
	while ( readed = file.Read(&buff[0], buff.size()) ) {

		if (readed < sizeof(rar::FILE_HEADER)) {
			return -1;
		}
	
		size_t pos = 0;
		while ( ((pos + sizeof(rar::FILE_HEADER))) <= readed ) {
			header = (rar::FILE_HEADER *)&buff[pos];
			if (IsValidFileHeader(header)) {
				
				if ((pos + header->header_size) <= readed) {					
					if (IsValidFileHeaderCrc(header)) {
						return offset;
					}			
				} else {					
					if (offset + header->header_size <= file_size) {
						break;
					} 				
				}			
			}
			pos++;
			offset++;
		}
		
		if (!file.SetPointer(offset)) {
			return -1;
		}
	}

	return -1;
}

int RarRecovery(const std::string &arch_path, const std::string &out_arch_path)
{
	W32Lib::FileEx origin_arch(arch_path.c_str());
	if (!origin_arch.Open()) {
		return - 1;
	}

	int64_t offset = FindFileHeader(origin_arch);
	if (offset != -1) {

		W32Lib::FileEx out_file(out_arch_path.c_str());
		if (!out_file.Create()) {
			return -1;
		}

		uint8_t rar_header[] = { 0x52, 0x61, 0x72, 0x21, 0x1A, 0x07, 0x00, 0xCF, 0x90, 0x73, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		if (out_file.Write(rar_header, sizeof(rar_header)) != sizeof(rar_header)) {
			return -1;
		}

		if (!origin_arch.SetPointer(offset)) {
			return -1;
		}
	
		std::vector<uint8_t> buff(128*1024, 0);
		size_t readed = 0;

		while (readed = origin_arch.Read(&buff[0], buff.size())) {
			out_file.Write(&buff[0], readed);
		}
		return 0;
	}

	return -1;
}

int RarRecoveryMain(int argc, _TCHAR* argv[])
{
	std::cout << "Rar recovery v. 0.0.2\n";
	//std::cout << "argc: " << argc << std::endl;
	//for (int i = 1; i < argc; i++) {
	//	std::cout << "arg: " << argv[i] << std::endl;
	//}

	if (argc >= 3) {
		//std::string arch_path = "F:\\Root\\$RECYCLE.BIN\\$RDQ36MA\\Чижовка\\ЗВІТ_ГЕС_Чижовка_нормальний1.rar.bad";
		//std::string out_path = "F:\\Root\\$RECYCLE.BIN\\$RDQ36MA\\Чижовка\\ЗВІТ_ГЕС_Чижовка_нормальний1.rar";

		std::string arch_path = argv[CMD_ARG_SOURCE_FILE];
		std::string out_path = argv[CMD_ARG_OUT_FILE];

		if (RarRecovery(arch_path, out_path) == 0) {
			std::cout << "Finished. RAR restored\n";
		} else {
			std::cout << "Finished. Failed to restore the RAR archive.\n";
		}
	} else {
		std::cout << "USAGE:  rarec <src_arch_path> <out_arch_path>\n";
	}
	return 0;
}


void ExamineFile(const std::string &arch_path)
{
	W32Lib::FileEx io(arch_path.c_str());
	if (!io.Open()) {
		return;
	}

	rar::FILE_HEADER hdr = { 0 };

	LONGLONG offset = 20;

	size_t sz = sizeof(rar::FILE_HEADER);
	

	while (true) {
		

		io.SetPointer(offset);
		io.Read(&hdr, sizeof(hdr));


		if (IsValidFileHeader(&hdr)) {

			size_t block_size = 0;

			if (hdr.type == rar::BlockType::FileHeader) {
				offset += hdr.header_size + hdr.compressed_size;
			} else {
				int x = 0;
			}


		
			int y = 0;
		} else {
			int x = 0;
			
		}
		
	
	
	}




}