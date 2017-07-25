#include "RarRecovery.h"
#include "crc32.h"
#include "W32Lib.h"
#include <vector>

bool IsFileHeader(rar::FILE_HEADER *header)
{
	if (header->type != rar::BlockType::FileHeader) {
		return false;
	}

	if (header->uncompressed_size) {
		if (header->compressed_size > header->uncompressed_size) {
			return false;
		}
	} else {
		if (header->compressed_size != 0) {
			return false;
		}		
	}

	if (header->os > rar::OperatingSystem::BeOS) {	
		return false;
	}

	size_t pack = header->packing_mathod;
	if (!((pack >= rar::PackingMethod::Storing)&&(pack <= rar::PackingMethod::BestCompression))) {
		return false;
	}

	uint32_t crc = crc32(&header->type, header->header_size - 2);
	if ((uint16_t)crc != header->crc) {
		return false;
	}
	
	return true;
}

int RarRecovery(const std::string &arch_path, const std::string &out_arch_path)
{
	W32Lib::FileEx file("D:\\Src\\vmfs.rar");
	if (!file.Open()) {
		return - 1;
	}

	std::vector<uint8_t> buff(512, 0);

	rar::FILE_HEADER *header = (rar::FILE_HEADER *)&buff[0];

	file.SetPointer(0x14);
	file.Read(buff.data(), sizeof(rar::FILE_HEADER));
	if (header->header_size > sizeof(rar::FILE_HEADER)) {
		file.Read(&buff[sizeof(rar::FILE_HEADER)], header->header_size - sizeof(rar::FILE_HEADER));		
	}

	char *file_name = header->file_name;


	bool result = IsFileHeader(header);

	LONGLONG pos = 0x14;

	while (1) {
	
		file.SetPointer(pos);
		file.Read(buff.data(), sizeof(rar::FILE_HEADER));
		if (header->header_size > sizeof(rar::FILE_HEADER)) {
			file.Read(&buff[sizeof(rar::FILE_HEADER)], header->header_size - sizeof(rar::FILE_HEADER));
		}

		if (IsFileHeader(header)) {		
			pos += header->header_size + header->compressed_size;
		} else {
			break;
		}	
	
	}	

	return 0;
}
