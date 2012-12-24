#include "mover.h"
#include <assert.h>
#include "W32Lib.h"

namespace mover
{

#define MOV_HEADER_SIZE					0x18

#define SECTOR_SIZE					(DWORD)512
#define MAX_MOV_SIZE				(LONGLONG)30*1024*1024*1024	// max size: 30 Gb

#define BUFFER_SIZE					(DWORD)256*SECTOR_SIZE

	typedef struct _MOV_HEADER {
		DWORD size;
		DWORD type;
	} MOV_HEADER;

	typedef struct _MOV_ATOM {
		MOV_HEADER hdr;
		BYTE data[1];
	} MOV_ATOM;

	class OutStream
	{
		File *file;
		LONGLONG base_offset;
		LONGLONG buffer_offset;
		BYTE *buffer;
		DWORD buffer_size;
		DWORD offset;
		DWORD readed;

	public:
		OutStream(File *_file, LONGLONG &_offset) : 
			file(_file),
			base_offset(_offset),
			buffer_offset(_offset),
			buffer(NULL),
			offset(0),
			readed(0)
		{
			assert(file);
			if(!file->IsOpen())
				if (!file->Open())
					throw 0;
			if (!file->SetPointer(base_offset))
				throw 0;

			buffer_size = 256*SECTOR_SIZE;
			buffer = new BYTE[buffer_size];
		}

		~OutStream()
		{
			if (buffer) delete[] buffer;
		}

		DWORD Read(BYTE *buff, DWORD count)
		{
			DWORD cnt = 0;
			DWORD out_buff_offset = 0;
			while (count) {
				cnt = min(count, (readed - offset));
				if (cnt) {
					memcpy(&buff[out_buff_offset], &buffer[offset], cnt);
					offset += cnt;
					out_buff_offset += cnt;
					count -= cnt;
				}
				else {
					offset = 0;
					buffer_offset += readed;
					if (!file->SetPointer(buffer_offset))
						break;
					if (!(readed = file->Read(buffer, buffer_size)))
						break;
				}
			}
			return out_buff_offset;
		}

		void BytesReaded(LONGLONG &readed)
		{
			readed = buffer_offset + offset;	
		}
	};

	DWORD Be2Le(DWORD &be)
	{
		DWORD le;
		((BYTE *)&le)[0] = ((BYTE *)&be)[3];
		((BYTE *)&le)[1] = ((BYTE *)&be)[2];
		((BYTE *)&le)[2] = ((BYTE *)&be)[1];
		((BYTE *)&le)[3] = ((BYTE *)&be)[0];
		return le;
	}

	BOOL PrepareDirPath(TCHAR *path, TCHAR *prep_path)
	{
		if (_tcslen(path) <= (MAX_PATH - 2)) {
			_tcscpy_s(prep_path, MAX_PATH, path);
			if (prep_path[_tcslen(path) - 1] != '\\') {
				prep_path[_tcslen(path)] = '\\';
				prep_path[_tcslen(path) + 1] = '\0';
			}
			return TRUE;
		}
		return FALSE;
	}

	void PrepareNextFileName(TCHAR *file_name, TCHAR *dir, LONGLONG &lba)
	{
		assert(file_name && dir);
		static DWORD file_counter;
		memset(file_name, 0x00, sizeof(TCHAR)*MAX_PATH);
		_stprintf_s(file_name, MAX_PATH, _T("%s%09lld.mov"), dir, lba);
		file_counter++;
	}

	BOOL IsValidMovHeader(MOV_HEADER *hdr)
	{
		assert(hdr);
		return ('ftyp' == Be2Le(hdr->type));
		//return ((MOV_HEADER_SIZE == Be2Le(hdr->size)) && ('ftyp' == Be2Le(hdr->type)));
	}

	BOOL FindNextMovHeader(File *file, LONGLONG *offset)
	{
		assert(file);

		DWORD rw = 0;
		LONGLONG saved_offs = 0;
		LONGLONG offs = 0;
		MOV_HEADER *hdr = NULL;
		BYTE *buff = new BYTE[BUFFER_SIZE];

		file->GetPointer(&saved_offs);
		offs = saved_offs;
		while (rw = file->Read(buff, BUFFER_SIZE)) {
			DWORD loop = rw/SECTOR_SIZE;
			for (DWORD i = 0; i < loop; i++) {
				hdr = (MOV_HEADER *)&buff[i*SECTOR_SIZE];
				if (IsValidMovHeader(hdr)) {
					file->SetPointer(offs);
					if (offset)	*offset = offs;
					return TRUE;
				}
				offs += SECTOR_SIZE;
			}	
		}
		file->SetPointer(saved_offs);
		delete[] buff;
		return FALSE;
	}

	BOOL IsValidAtom(MOV_ATOM *atom)
	{
		switch (Be2Le(atom->hdr.type)) {
			case 'ftyp':
			case 'moov':
			case 'mvhd':
			case 'mdat':
			case 'skip':
			case 'free':
			case 'wide':
			case 'trak':
			case 'tkhd':
				return TRUE;
			default :
				return FALSE;
		}
	}

	//
	// Сохраняет атомы в файл до тех пор пока:
	//  - не встретит неизвестный тип атома
	//  - не встретит хидер нового файла
	//  - не превысит допустимый размер выходного файла
	// 
	// После выполнения функции file_pointer указывает:
	//  - на следующий сектор после конца сохранённого файла
	//  - на новый хидер
	//
	BOOL SaveMovToFile(File &src_file,TCHAR *out_file_name, LONGLONG &max_size)
	{
		DWORD rw = 0;
		DWORD loop = 0;
		DWORD atom_data_size = 0;
		DWORD read_at_once = 0;
		MOV_ATOM *atom = NULL;
		MOV_HEADER *hdr = NULL;
		BYTE buff[BUFFER_SIZE] = {0};

		LONGLONG saved_offset = 0;
		LONGLONG atom_offset = 0;
		src_file.GetPointer(&saved_offset);
		atom_offset = saved_offset;

		FileEx out_file(out_file_name);
		if (!out_file.Create()) 
			return FALSE;

		OutStream stream(&src_file, saved_offset);

		//rw = stream.Read(buff, MOV_HEADER_SIZE);
		//out_file.Write(buff, MOV_HEADER_SIZE);

		while (sizeof(MOV_HEADER) == stream.Read(buff, sizeof(MOV_HEADER))) {
			atom = (MOV_ATOM *)buff;
			if (IsValidAtom(atom)) {
				out_file.Write(atom, sizeof(MOV_HEADER));
				atom_data_size = Be2Le(atom->hdr.size) - sizeof(MOV_HEADER);
				while (atom_data_size) {
					read_at_once = min(BUFFER_SIZE, atom_data_size);
					rw = stream.Read(buff, read_at_once);
					out_file.Write(buff, rw);
					atom_data_size -= rw;
				}
			}
			else
				break;
		}

		return TRUE;
	}

	int mover_run(File &file, TCHAR *out_dir)
	{
		TCHAR out_file_name[MAX_PATH] = {0};
		LONGLONG hdr_offset = 0;
		LONGLONG hdr_lba = 0;
		LONGLONG max_file_size = MAX_MOV_SIZE;

		while (FindNextMovHeader(&file, &hdr_offset)) {
			hdr_lba = (hdr_offset/512);
			_tprintf(_T("Valid HEADER at #%09lld\n"), hdr_lba);
			PrepareNextFileName(out_file_name, out_dir, hdr_lba);
			SaveMovToFile(file, out_file_name, max_file_size);
			if (!file.SetPointer(hdr_offset + SECTOR_SIZE)) break;
		}
		return 0;
	}

}

int mover_main(TCHAR *file_name, TCHAR *out_dir)
{
	assert(file_name && out_dir);

	File file(file_name);
	TCHAR out_dir_path[MAX_PATH] = {0};
	if (file.Open() && mover::PrepareDirPath(out_dir, out_dir_path))
		return mover::mover_run(file, out_dir_path);
	return -1;
}