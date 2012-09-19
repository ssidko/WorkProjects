#include "ZipRec.h"


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

int PrepareAndExtract(FileEx *archive, TCHAR *out_dir)
{
	archive->SetPointer(0x00);

	DWORD rw = 0;
	LONGLONG descr_offs = 0;

	int x = sizeof(LOCAL_FILE_HEADER_32);
	
	BYTE *h_buff = new BYTE[2048];
	LOCAL_FILE_HEADER_32 *header = (LOCAL_FILE_HEADER_32 *)h_buff;
	memset(h_buff, 0x00, 2048);

	if(sizeof(LOCAL_FILE_HEADER_32) != archive->Read(h_buff, sizeof(LOCAL_FILE_HEADER_32)))
		return 0;

	archive->Read(&h_buff[sizeof(LOCAL_FILE_HEADER_32)], header->name_len + header->extra_field_len - 1);

	if ( (header->flag & (1 << 3)) )
	{
		DWORD descr_sign = DATA_DESCRIPTOR_SIGN;
		if ( (descr_offs = archive->Find((BYTE *)&descr_sign, sizeof(descr_sign))) != -1 )
		{
			DATA_DESCRIPTOR_32 data_descr;
			memset(&data_descr, 0x00, sizeof(DATA_DESCRIPTOR_32));
			archive->Read(&data_descr, sizeof(DATA_DESCRIPTOR_32));
				
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
	DWORD ch_sz = sizeof(CENTRAL_FILE_HEADER_32) + central_header->comment_len + central_header->extra_field_len + central_header->name_len - 1;

	BYTE *er_buff = new BYTE[2048];
	memset(er_buff, 0x00, 2048);
	END_OF_CDIRECTORY_RECORD_32 *end_record = (END_OF_CDIRECTORY_RECORD_32 *)er_buff;

	end_record->signature = END_CDIRECTORY_RECORD_SIGN;
	end_record->cdir_entries_count = 1;
	end_record->cdir_entries_total_count = 1;
	end_record->cdir_size = ch_sz;
	end_record->cdir_offset = sizeof(LOCAL_FILE_HEADER_32) + header->name_len + header->extra_field_len - 1 + header->compr_size;
	end_record->comment_len = 0;

	archive->SetPointer(0x00);
	archive->Write(header, sizeof(LOCAL_FILE_HEADER_32));

	archive->SetPointer(end_record->cdir_offset);
	archive->Write(central_header, ch_sz);
	archive->Write(end_record, sizeof(END_OF_CDIRECTORY_RECORD_32));
	archive->Close();

	TCHAR cmd_str[1024] = {0};
	//_stprintf_s(cmd_str, 1024, _T("\"C:\\Program Files\\7-Zip\\7z.exe\" x -y -o%s %s"), out_dir, archive->GetName());

	_stprintf_s(cmd_str, 1024, _T(".\\7z.exe x -y -o%s %s"), out_dir, archive->GetName());

	//::system(cmd_str);

	delete h_buff;
	delete ch_buff;
	delete er_buff;
	return 0;
}

BOOL IsValidLocalFileHeader(LOCAL_FILE_HEADER_32 *header)
{
	if (header->signature != LOCAL_FILE_HEADER_SIGN)
		return FALSE;

	if (!((header->compr_method >= 0) && (header->compr_method <= 19)) && 
		!(header->compr_method == 97) &&
		!(header->compr_method == 98) )
		return FALSE;

	if (header->flag & (1 << 3))
	{
		if ((header->crc32 != 0) || (header->compr_size != 0) || (header->uncompr_size != 0))
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

int ExtractArchive(FileEx *archive, TCHAR *out_dir)
{
	DWORD rw = 0;
	LONGLONG prev_offs = 0;
	LONGLONG curr_offs = 0;
	DWORD file_sign = LOCAL_FILE_HEADER_SIGN;

	TCHAR file_name[1024] = {0};
	//_stprintf_s(file_name, 1024, _T("%s\\tmp.zip"), out_dir);
	_stprintf_s(file_name, 1024, _T(".\\tmp.zip"), out_dir);

	if ( (prev_offs = FindNextLocalFile(archive)) != -1 ) 
	{
		archive->SetPointer(prev_offs + 1);
		while ( (curr_offs = FindNextLocalFile(archive)) != -1 )
		{

			DWORD file_size = (DWORD)(curr_offs - prev_offs);
			BYTE *buff = new BYTE[file_size];
			memset(buff, 0x00, file_size);

			FileEx out_file(file_name);
			if (out_file.Create())
			{
				archive->SetPointer(prev_offs);
				rw = archive->Read(buff, file_size);
				out_file.Write(buff, rw);

				_tprintf(_T("Offset: %lld\n"), prev_offs);
				PrepareAndExtract(&out_file, out_dir);
			}
			
			delete[] buff;

			prev_offs = curr_offs;
			archive->SetPointer(curr_offs + 1);
		}
	}
	return 0;
}
