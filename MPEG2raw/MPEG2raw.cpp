#include "stdafx.h"
#include <conio.h>
#include "W32Lib.h"

using namespace W32Lib;

#define SECTOR_SIZE				512
#define MPEG_SIGNATURE			((DWORD)0xBA010000)
#define MAX_STR_LEN				255

BOOL IsMPEG(BYTE *buff)
{
	return ((*(DWORD *)buff) == MPEG_SIGNATURE);
}

BOOL PrepareDirPath(TCHAR *raw_dir, TCHAR *out_dir)
{
	DWORD length = _tcslen(raw_dir);
	memcpy(out_dir, raw_dir, length*sizeof(TCHAR));
	if (length && (length + 2 < MAX_STR_LEN))
	{
		TCHAR ch = out_dir[length - 1];
		if (ch != _T('\\'))
		{
			out_dir[length] = _T('\\');
			out_dir[length+1] = _T('\0');
		}
		return TRUE;
	}
	return FALSE;
}

BOOL FindNextMpegFrame(FileEx *file)
{
	if (!file->IsOpen())
		return FALSE;

	LONGLONG offset = 0;
	if (!file->GetPointer(&offset))
		return FALSE;

	DWORD rw = 0;
	BYTE *buff = new BYTE[255*SECTOR_SIZE];
	BYTE *sector = buff;
 
	while (rw = file->Read(buff, 255*SECTOR_SIZE))
	{
		for (DWORD i = 0; i < (rw/SECTOR_SIZE); i++)
		{
			sector = &buff[i*SECTOR_SIZE];
			if (IsMPEG(sector))
			{
				file->SetPointer(offset);
				delete buff;
				return TRUE;
			}
			offset += SECTOR_SIZE;
		}
	}
	return FALSE;
}	

BOOL IsExistNextFrame(FileEx *file)
{
	if (!file->IsOpen())
		return FALSE;

	LONGLONG offset = 0;
	if (file->GetPointer(&offset)) { 
		BYTE buff[SECTOR_SIZE] = {0};
		if (file->Read(buff, SECTOR_SIZE) == SECTOR_SIZE) {
			if (IsMPEG(buff)) {
				file->SetPointer(-SECTOR_SIZE, FILE_CURRENT);
				return TRUE;
			}
		}
	}

	return FALSE;
}

int Run(TCHAR *file_name, TCHAR *dir)
{
	DWORD rw = 0;
	DWORD counter = 0;
	BYTE buff[512] = {0};
	BYTE frame[2048] = {0};
	TCHAR out_f_name[MAX_STR_LEN] = {0};

	FileEx src_file(file_name);
	if (!src_file.Open())
		return -1;

	FileEx *out_file = NULL;

	while (FindNextMpegFrame(&src_file))
	{
		LONGLONG lba;
		src_file.GetPointer(&lba);
		_tprintf(_T("%06d\n"), lba/512);

		if (!out_file) {
			_stprintf_s(out_f_name, MAX_STR_LEN, _T("%s%06d.mpg"), dir, counter);
			out_file = new FileEx(out_f_name);
			if (!out_file->Create()) return -1;
		}

		do {
			src_file.Read(frame, 2048);
			out_file->Write(frame, 2048);		
		} while (IsExistNextFrame(&src_file));

		delete out_file;
		out_file = NULL;
		counter++;
	}	
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	//FileEx src_file(_T("D:\\Work\\29445\\DEM_0000.MPG"));
	//if (src_file.Open())
	//{
	//	LONGLONG offset = 0;
	//	while (FindNextMpeg(&src_file))
	//	{
	//		src_file.GetPointer(&offset);
	//		src_file.SetPointer(SECTOR_SIZE, FILE_CURRENT);
	//	}
	//}


	if (argc < 3)
	{
		_tprintf(_T("\nUsage: <input_file> <output_dir>"));
		_getch();
		return 0;
	}
	else
	{
		TCHAR out_dir[MAX_STR_LEN] = {0};
		PrepareDirPath(argv[2], out_dir);
		Run(argv[1], out_dir);
	}

	_tprintf(_T("\n Done. Press any key ..."));
	_getch();

	return 0;
}

