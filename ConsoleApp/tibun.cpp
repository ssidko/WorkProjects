#include "tibun.h"

int tibun_main(int argc, _TCHAR* argv[])
{
	if (argc >= 3)
	{
		FileEx src_file(argv[1]);
		FileEx out_file(argv[2]);
		if (src_file.Open()) {
			if (out_file.Create()) {
				return Unpack(&src_file, &out_file);
			}
			else
				_tprintf(_T("Cannot create file %s\n"), argv[2]);
		}
		else
			_tprintf(_T("Cannot open file %s\n"), argv[1]);
	}
	else
		_tprintf(_T("Usage: <*.tib> <out file>\n"));

	return -1;
}

int Unpack(FileEx *src_file, FileEx *out_file)
{
	//BYTE mark[MARK_SIZE] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	BYTE mark[MARK_SIZE] = {0x49, 0x44, 0x41, 0x54};
	LONGLONG prev_mark = 0;
	LONGLONG curr_mark = 0;
	LONGLONG counter = 0;
	int ret = 0;
	
	DWORD rw = 0;
	DWORD unpack_data_size = UNPACK_DATA_SIZE;
	BYTE *unpack_buff = new BYTE[UNPACK_DATA_SIZE];
	BYTE *pack_buff = new BYTE[UNPACK_DATA_SIZE];
	memset(unpack_buff, 0x00, 2048*512);

	BYTE buffeee[8192] = {0};


	if (src_file->Find(mark, MARK_SIZE) != -1)
	{
		DWORD size = 0;
		src_file->SetPointer(MARK_SIZE, FILE_CURRENT);
		rw = src_file->Read(buffeee, 8192);
		//ret = uncompress(unpack_buff, &unpack_data_size, buffeee, 8192);
		if(ret == Z_OK)
		{
			out_file->Write(unpack_buff, unpack_data_size);
			int x = 0;
		}
	
	}


	//if ((prev_mark = src_file->Find(mark, MARK_SIZE)) != -1)
	//{
	//	src_file->SetPointer(prev_mark + 1);
	//	while ((curr_mark = src_file->Find(mark, MARK_SIZE)) != -1)
	//	{
	//		if (prev_mark == 9140261)
	//			int x = 0;

	//		unpack_data_size = UNPACK_DATA_SIZE;
	//		DWORD size = (DWORD)(curr_mark - prev_mark - 8);
	//		memset(pack_buff, 0x00, UNPACK_DATA_SIZE);

	//		src_file->SetPointer(prev_mark + 8);
	//		src_file->Read(pack_buff, size);

	//		ret = uncompress(unpack_buff, &unpack_data_size, pack_buff, size);
	//		if (ret == Z_OK)
	//		{
	//			_tprintf(_T("Offset: %lld\n"), prev_mark);
	//			out_file->Write(unpack_buff, unpack_data_size);
	//			int x = 0;
	//		}
	//		else
	//		{
	//			int x = 0;
	//		}
	//		
	//		//switch (ret) {			
	//		//case Z_OK :
	//		//case Z_DATA_ERROR :
	//		//	_tprintf(_T("Offset: %lld\n"), prev_mark);
	//		//	out_file->Write(unpack_buff, unpack_data_size);
	//		//	break;
	//		//default:
	//		//	break;
	//		//}

	//		memset(unpack_buff, 0x00, UNPACK_DATA_SIZE);
	//		prev_mark = curr_mark;
	//		src_file->SetPointer(curr_mark + 1);
	//	}				
	//}

	return 0; 
}
