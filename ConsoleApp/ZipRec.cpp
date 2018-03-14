#include "ZipRec.h"
#include <vector>
#include <iostream>

BOOL LocalFile::Initialize()
{
	LOCAL_FILE_HEADER_32 header;
	memset(&header, 0x00, sizeof(LOCAL_FILE_HEADER_32));

	if (archive->IsOpen())
	{
		LONGLONG save_offset;
		if (archive->GetPointer(&save_offset))
		{
			DWORD rw = 0;
			if (rw == archive->Read(&header, sizeof(LOCAL_FILE_HEADER_32)))
			{
				if ((header.flag & (1<<3)))
				{
					// должен быть DATA_DESCRIPTOR
				}
				archive->SetPointer(save_offset);
				return TRUE;
			}
			else
			{
				archive->SetPointer(save_offset);			
				return FALSE;
			}
		}
	}
	return FALSE;
}

int ZipRec_Main(int argc, _TCHAR* argv[])
{
	if (argc >= 3)
	{
		FileEx archive(argv[1]);
		if(archive.Open())
			return ExtractArchive(&archive, argv[2]);
	}
	else
		_tprintf(_T("Usage: <archive> <out directory>\n"));

	return 0;
}

int PrepareAndExtract(FileEx &archive, const TCHAR *out_dir)
{
	archive.SetPointer(0x00);

	DWORD rw = 0;
	LONGLONG descr_offs = 0;

	int x = sizeof(LOCAL_FILE_HEADER_32);
	
	BYTE *h_buff = new BYTE[2048];
	LOCAL_FILE_HEADER_32 *header = (LOCAL_FILE_HEADER_32 *)h_buff;
	memset(h_buff, 0x00, 2048);

	if(sizeof(LOCAL_FILE_HEADER_32) != archive.Read(h_buff, sizeof(LOCAL_FILE_HEADER_32)))
		return 0;

	archive.Read(&h_buff[sizeof(LOCAL_FILE_HEADER_32)], header->name_len + header->extra_field_len - 1);

	if ( (header->isHasDataDescriptor()) )
	{
		DWORD descr_sign = DATA_DESCRIPTOR_SIGN;
		if ( (descr_offs = archive.Find((BYTE *)&descr_sign, sizeof(descr_sign))) != -1 )
		{
			DATA_DESCRIPTOR_32 data_descr = {0};
			// memset(&data_descr, 0x00, sizeof(DATA_DESCRIPTOR_32));
			archive.Read(&data_descr, sizeof(DATA_DESCRIPTOR_32));
				
			if ( descr_offs == ( sizeof(LOCAL_FILE_HEADER_32) + header->name_len + header->extra_field_len - 1 + data_descr.compr_size ) )
			{
				header->crc32 = data_descr.crc32;
				header->compr_size = data_descr.compr_size;
				header->uncompr_size = data_descr.uncompr_size;
			}
		}
		else
			return 0;	
	}

	BYTE *ch_buff = new BYTE[2048];
	memset(ch_buff, 0x00, 2048);
	CENTRAL_FILE_HEADER_32 *central_header = (CENTRAL_FILE_HEADER_32 *)ch_buff;

	central_header->signature = CENTRAL_FILE_HEADER_SIGN;
	central_header->ver_needed = 0x14;
	central_header->ver_made = 0x0A;
	central_header->flag = header->flag;
	central_header->compr_method = header->compr_method;
	central_header->time = header->time;
	central_header->date = header->date;
	central_header->crc32 = header->crc32;
	central_header->compr_size = header->compr_size;
	central_header->uncompr_size = header->uncompr_size;
	central_header->name_len = header->name_len;
	memcpy(central_header->file_name, header->file_name, header->name_len);
	DWORD ch_sz = sizeof(CENTRAL_FILE_HEADER_32) - 1 + central_header->comment_len + central_header->extra_field_len + central_header->name_len;

	BYTE *er_buff = new BYTE[2048];
	memset(er_buff, 0x00, 2048);
	END_OF_CDIRECTORY_RECORD_32 *end_record = (END_OF_CDIRECTORY_RECORD_32 *)er_buff;

	end_record->signature = END_CDIRECTORY_RECORD_SIGN;
	end_record->cdir_entries_count = 1;
	end_record->cdir_entries_total_count = 1;
	end_record->cdir_size = ch_sz;
	end_record->cdir_offset = sizeof(LOCAL_FILE_HEADER_32) - 1 + header->name_len + header->extra_field_len + header->compr_size;
	end_record->comment_len = 0;

	archive.SetPointer(0x00);
	archive.Write(header, sizeof(LOCAL_FILE_HEADER_32));

	archive.SetPointer(end_record->cdir_offset);
	archive.Write(central_header, ch_sz);
	archive.Write(end_record, sizeof(END_OF_CDIRECTORY_RECORD_32));
	archive.Close();

	TCHAR cmd_str[1024] = {0};
	//_stprintf_s(cmd_str, 1024, _T("\"C:\\Program Files\\7-Zip\\7z.exe\" x -y -o%s %s"), out_dir, archive->GetName());

	_stprintf_s(cmd_str, 1024, _T(".\\7z.exe x -y -o%s %s"), out_dir, archive.GetName());
	::system(cmd_str);

	delete h_buff;
	delete ch_buff;
	delete er_buff;
	return 0;
}

int PrepareAndExtract(FileEx &archive, const std::string &out_dir)
{


	return 0;
}

bool IsValidLocalFileHeader(LOCAL_FILE_HEADER_32 &header)
{
	if (header.signature != LOCAL_FILE_HEADER_SIGN)
		return FALSE;

	if (!((header.compr_method >= 0) && (header.compr_method <= 19)) && 
		!(header.compr_method == 97) &&
		!(header.compr_method == 98) )
		return FALSE;

	if (header.isHasDataDescriptor())
	{
		if ((header.crc32 != 0) || (header.compr_size != 0) || (header.uncompr_size != 0))
			return FALSE;
	}

	return TRUE;	
}

LONGLONG FindNextLocalFile(FileEx *archive)
{
	LONGLONG offset = 0;
	DWORD file_sign = LOCAL_FILE_HEADER_SIGN;

	while ((offset = archive->Find((BYTE *)&file_sign, sizeof(file_sign))) != -1)
	{
		LOCAL_FILE_HEADER_32 header;
		archive->Read(&header, sizeof(LOCAL_FILE_HEADER_32));
		if (IsValidLocalFileHeader(&header))
		{
			archive->SetPointer(offset);
			return offset;
		}
		else
			archive->SetPointer(offset+1);
		
	}
	return -1;
}

bool SaveFileFragmentToNewFile(FileEx &src_file, uint64_t offset, uint64_t size, FileEx &new_file)
{
	uint32_t io_block_size = 128 * 512;
	std::vector<uint8_t> buffer(io_block_size, 0x00);
	uint64_t remain = size;
	uint32_t to_read = 0;
	uint32_t readed = 0;

	src_file.SetPointer(offset);
	while (remain) {		
		to_read = static_cast<uint32_t>(remain > buffer.size() ? buffer.size() : remain);
		readed = src_file.Read(buffer.data(), to_read);
		if (readed == 0) {
			break;
		}
		new_file.Write(buffer.data(), readed);
		remain -= readed;		
	}
	
	return remain == 0;
}

bool ReadAndSaveLocalFile(FileEx &archive, uint64_t offset, FileEx &out_file)
{
	std::vector<uint8_t> buffer(LOCAL_FILE_HEADER_SIZE, 0x00);
	LOCAL_FILE_HEADER_32 *file_header = (LOCAL_FILE_HEADER_32 *)buffer.data();

	archive.SetPointer(offset);
	if (LOCAL_FILE_HEADER_SIZE != archive.Read(buffer.data(), LOCAL_FILE_HEADER_SIZE)) {
		return false;
	}

	if (!IsValidLocalFileHeader(*file_header)) {
		return false;	
	}

	size_t var_len = file_header->name_len + file_header->extra_field_len;
	buffer.resize(LOCAL_FILE_HEADER_SIZE + var_len);
	file_header = (LOCAL_FILE_HEADER_32 *)buffer.data();

	if (var_len != archive.Read(&buffer[LOCAL_FILE_HEADER_SIZE], var_len)) {
		return false;
	}



	return true;

}

int ExtractArchive(FileEx *archive, const TCHAR *out_dir)
{
	DWORD rw = 0;
	uint64_t prev_offs = 0;
	uint64_t curr_offs = 0;
	DWORD file_sign = LOCAL_FILE_HEADER_SIGN;


	uint64_t local_file_size = 0;
	uint64_t local_file_offset = 0;
	std::string tmp_file_name = ".\\tmp.zip";

	if ( (prev_offs = FindNextLocalFile(archive)) != -1 ) 
	{
		archive->SetPointer(prev_offs + 1);
		while ( (curr_offs = FindNextLocalFile(archive)) != -1 )
		{

			local_file_size = curr_offs - prev_offs;
			local_file_offset = prev_offs;
			FileEx tmp_file(tmp_file_name.c_str());
			if (tmp_file.Create()) {
				std::cout << std::endl << "Find FILE_HEADER at: " << local_file_offset << std::endl;

				if (!SaveFileFragmentToNewFile(*archive, local_file_offset, local_file_size, tmp_file)) {
					PrepareAndExtract(tmp_file, out_dir);					
				} else {
					std::cout << "Couldn't save zip_local_file to temp_file" << std::endl;
				}			

				prev_offs = curr_offs;
				archive->SetPointer(local_file_offset + 1);			
			}

		}
	}
	return 0;
}

int TestZipRec(void)
{
	std::string file_name = "E:\\43881\\PASSPORT.zip";
	std::string out_dir = "F:\\43881\\v2";

	FileEx archive(file_name.c_str());
	if (archive.Open()) {
		archive.SetPointer(46992060398LL);
		return ExtractArchive(&archive, out_dir.c_str());	
	}		

	return 0;

}
