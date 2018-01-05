#include "stdafx.h"
#include <vector>
#include <list>
#include <string>
#include <fstream>
#include "ntfs.h"
#include "ByteStream.h"
#include <zlib.h>
#include <iostream>

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

#define CHUNK_SIZE	512*512

/* Компрессия */
bool compress_file(FILE *src, FILE *dst)
{
	uint8_t inbuff[CHUNK_SIZE];
	uint8_t outbuff[CHUNK_SIZE];
	z_stream stream = { 0 };

	if (deflateInit(&stream, Z_BEST_SPEED) != Z_OK)
	{
		fprintf(stderr, "deflateInit(...) failed!\n");
		return false;
	}

	int flush;
	do {
		stream.avail_in = fread(inbuff, 1, CHUNK_SIZE, src);
		if (ferror(src))
		{
			fprintf(stderr, "fread(...) failed!\n");
			deflateEnd(&stream);
			return false;
		}

		flush = feof(src) ? Z_FINISH : Z_NO_FLUSH;
		stream.next_in = inbuff;

		do {
			stream.avail_out = CHUNK_SIZE;
			stream.next_out = outbuff;
			deflate(&stream, flush);
			uint32_t nbytes = CHUNK_SIZE - stream.avail_out;

			if (fwrite(outbuff, 1, nbytes, dst) != nbytes || ferror(dst))
			{
				fprintf(stderr, "fwrite(...) failed!\n");
				deflateEnd(&stream);
				return false;
			}
		} while (stream.avail_out == 0);
	} while (flush != Z_FINISH);

	deflateEnd(&stream);
	return true;
}

/* Декомпрессия */
bool decompress_file(FILE *src, FILE *dst)
{
	uint8_t inbuff[CHUNK_SIZE] = {0};
	uint8_t outbuff[CHUNK_SIZE] = {0};
	z_stream stream = { 0 };
	bool eof = false;

	int result = Z_OK;
	//int result = inflateInit(&stream);
	//if (result != Z_OK)
	//{
	//	fprintf(stderr, "inflateInit(...) failed!\n");
	//	return false;
	//}

	fpos_t prev_pos = 0;
	fpos_t pos = 0;
	fgetpos(src, &pos);

	do {

		z_stream stream = { 0 };
		result = inflateInit(&stream);
		if (result != Z_OK)
		{
			fprintf(stderr, "inflateInit(...) failed!\n");
			return false;
		}

		do {
			stream.avail_in = fread(inbuff, 1, CHUNK_SIZE, src);
			if (ferror(src))
			{
				fprintf(stderr, "fread(...) failed!\n");
				inflateEnd(&stream);
				return false;
			}

			if (stream.avail_in == 0) {
				eof = true;
				break;
			}

			stream.next_in = inbuff;

			do {
				stream.avail_out = CHUNK_SIZE;
				stream.next_out = outbuff;
				result = inflate(&stream, Z_NO_FLUSH);
				if (result == Z_NEED_DICT || result == Z_DATA_ERROR ||
					result == Z_MEM_ERROR)
				{
					fprintf(stderr, "inflate(...) failed: %d\n", result);
					fgetpos(src, &pos);
					std::cout << stream.msg << std::endl;
					std::cout << "Pos: " << std::to_string(pos) << std::endl;
					inflateEnd(&stream);
					return false;
				}

				uint32_t nbytes = CHUNK_SIZE - stream.avail_out;

				if (fwrite(outbuff, 1, nbytes, dst) != nbytes || ferror(dst))
				{
					fprintf(stderr, "fwrite(...) failed!\n");
					inflateEnd(&stream);
					return false;
				}
			} while (stream.avail_out == 0);
		} while (result != Z_STREAM_END);

		prev_pos = pos;
		pos += stream.total_in + 8;
		fsetpos(src, &pos);

		inflateEnd(&stream);
	
	} while (!eof);


	//inflateEnd(&stream);
	return result == Z_STREAM_END;
}

#include "File.h"
#include <exception>
#include <memory>

std::unique_ptr<W32Lib::FileEx> CreateNewFile(std::string &dir)
{
	static size_t file_counter = 0;
	std::string out_file_name = dir + std::to_string(file_counter++) + ".bin";
	std::unique_ptr<W32Lib::FileEx> out_file(new W32Lib::FileEx(out_file_name.c_str()));
	if (!out_file->Create()) {
		throw std::runtime_error("Error create out file");
	}
	return out_file;
}

bool ReadChunk(W32Lib::FileEx &io, std::vector<uint8_t> &buffer)
{

	return false;
}

int _tmain(int argc, _TCHAR* argv[])
{


	FILE * src = fopen("E:\\43410\\examles\\Win8Backup_full_b1_s1_v1.tib", "rb");
	FILE * dst = fopen("F:\\out.bin", "wb");

	//compress_file(src, dst);
	fseek(src, 32 + 8, SEEK_SET);
	//fseek(src, 0x1003a6d6, SEEK_SET);	

	fpos_t pos = 0;

	decompress_file(src, dst);

	fclose(src);
	fclose(dst);

	std::string tib_file_name = "E:\\43410\\examles\\Win8Backup_full_b1_s1_v1.tib";
	std::string out_folder = "F:\\43410\\out\\";
	
	W32Lib::FileEx tib_file(tib_file_name.c_str());
	if (tib_file.Open()) {
		
		ULONGLONG separator = 0xffffffffffffffff;
		LONGLONG offset = 0;
		size_t readed = 0;

		std::vector<uint8_t> uncompressed(CHUNK_SIZE, 0);
		std::vector<uint8_t> compressed(CHUNK_SIZE, 0);
	
		while (offset = tib_file.Find((BYTE *)&separator, sizeof(separator))) {
		
			std::unique_ptr<W32Lib::FileEx> out_file = CreateNewFile(out_folder);
		
		}

	}


	attr_list attributes;
	std::string attr_list_file = "D:\\Work\\43410\\attr_list.bin";
	std::string mft_file = "D:\\Work\\43410\\MFT.bin";

	ReadAttributeListFromFile(attr_list_file, attributes);

	uint8_t buff[] = { 0x33, 0x11, 0x22, 0x33, 0x44, 0x55, 0x80 };
	ByteStream stream(buff, sizeof(buff));

	extent ext = { 0 };
	ReadExtent(stream, ext);

	return 0;
}

