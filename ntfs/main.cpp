#include "stdafx.h"
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include "ntfs.h"
#include "ByteStream.h"

typedef struct {
	uint64_t start_lcn;
	uint64_t count;
} extent;

typedef struct {
	uint8_t count_len : 4;
	uint8_t lcn_len : 4;

	operator bool() const
	{
		return count_len && lcn_len;
	}
} extent_hdr;

using attr_list = std::vector<ATTR_LIST_ENTRY>;
using run_list = std::list<extent>;


bool ReadAttributeListFromFile(const std::string &attr_list_file_path, attr_list &attr)
{
	attr.clear();

	std::fstream file;
	file.open(attr_list_file_path, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	if (file.is_open()) {
		ATTR_LIST_ENTRY entry;
		do {
			std::memset(&entry, 0x00, sizeof(ATTR_LIST_ENTRY));
			file.read(reinterpret_cast<char *>(&entry), sizeof(entry));
			if (!file.eof() && !file.bad()) {
				attr.push_back(entry);
			}
		} while (!file.eof() && !file.bad());
		return attr.size() ? true : false;
	}
	return false;
}

bool ReadMftRecord(const std::string& mft_file, uint64_t record_num, std::vector<uint8_t> &record)
{
	const int mft_record_size = 1024;
	const int sector_size = 512;

	std::fstream mft;
	mft.open(mft_file, std::ios_base::in | std::ios_base::out | std::ios_base::binary);
	if (mft.is_open()) {

		mft.seekg(record_num * mft_record_size);

	}
	return false;
}

bool ReadExtent(ByteStream &stream, extent &ext)
{
	extent_hdr hdr = { 0 };
	if (1 != stream.Read((uint8_t *)&hdr, 1)) {
		return false;
	}

	if (!hdr) {
		return false;
	}

	size_t count_len = hdr.count_len;
	size_t lcn_len = hdr.lcn_len;

	if ((count_len) > 8 || (lcn_len > 8)) {
		return false;
	}

	if ((count_len + lcn_len) <= stream.Remain()) {

		stream.Read((uint8_t *)&ext.count, count_len);
		stream.Read((uint8_t *)&ext.start_lcn, lcn_len);

		int8_t lcn_msb = ((int8_t *)&ext.start_lcn)[lcn_len - 1];
		if (lcn_msb < 0) {

			for (size_t i = 0; i < lcn_len; i++) {


			}

		}

		return true;
	}

	return false;
}

bool DecodeRunlist(uint8_t *buff, size_t buff_size, run_list &extents)
{
	extents.clear();

	extent_hdr hdr = { 0 };
	uint64_t vcn = 0;
	size_t count_len = 0;
	size_t offset_len = 0;
	ByteStream stream(buff, buff_size);

	while (stream.Read((uint8_t *)&hdr, 1)) {

		if (!hdr) {
			break;
		}

		extent ext = { 0 };

		count_len = hdr.count_len;;
		offset_len = hdr.lcn_len;

		if (count_len != stream.Read((uint8_t *)&ext.count, count_len)) {
			return false;
		}
		if (offset_len != stream.Read((uint8_t *)&ext.start_lcn, offset_len)) {
			return false;
		}

		vcn += 0;

	}

	return false;
}

int _tmain(int argc, _TCHAR* argv[])
{
	attr_list attributes;
	std::string attr_list_file = "d:\\MyProjects\\examles\\attr_list.bin";
	std::string mft_file = "d:\\MyProjects\\examles\\MFT.bin";

	ReadAttributeListFromFile(attr_list_file, attributes);

	uint8_t buff[] = { 0x33, 0x11, 0x22, 0x33, 0x44, 0x55, 0x80 };
	ByteStream stream(buff, sizeof(buff));

	extent ext = { 0 };
	ReadExtent(stream, ext);

	return 0;
}

