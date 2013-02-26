#include "h264.h"
#include <list>

namespace h264dvr
{

#define SECTOR_SIZE				(DWORD)512

class OutStream
{
private:
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

		  buffer_size = 128*SECTOR_SIZE;
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

#pragma pack (push)
#pragma pack (1)

typedef struct _VHDR {
	DWORD sign : 24;
	DWORD type : 4;
	DWORD flag : 4;
} VHDR;

typedef struct _FRAME {
	VHDR hdr;
	DWORD unk;
	DWORD time;
	DWORD size;
} FRAME;
#pragma pack(pop)

class VideoFile
{
private:
	DWORD begin_time;
	DWORD last_time;
	FileEx io;
public:
	VideoFile(TCHAR *file_name) : io(file_name), begin_time(0), last_time(0) {if (!io.Create()) throw 0;}
	BOOL SaveFrame (FRAME *frame)
	{
		if (!begin_time) {
			begin_time = frame->time;
		}
		last_time = frame->time;
		return ((frame->size + 16) == io.Write((BYTE *)frame, (frame->size + 16)));
	}
	DWORD BeginTime(void) {return begin_time;}
	DWORD LastTime(void) {return last_time;}
};

class VideoStorage
{
private:
	DWORD file_counter;
	BYTE out_dir[MAX_PATH];
public:
	VideoStorage(TCHAR *path) : file_counter(0)
	{

	}

	void SaveFrame (FRAME *frame)
};


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

int Run(OutStream &io)
{
	return 0;
}

int main(TCHAR *file_name, LONGLONG offset, TCHAR *out_dir)
{
	assert(file_name && out_dir);

	File file(file_name);
	if (file.Open()) {
		OutStream io(&file, offset);
		TCHAR dir_path[MAX_PATH] = {0};
		PrepareDirPath(out_dir, dir_path);

		return Run(io);
	}
	return -1;
}

// min_time = 882212390;

void testing(void)
{
	FileEx file(_T("J:\\Work\\33275\\tst.h264"));
	if (file.Open()) {
		VHDR hdr;
		DWORD size = 0;
		DWORD unk = 0;
		DWORD time = 0;
		LONGLONG chunk_offset = 0;
		while (sizeof(VHDR) == file.Read(&hdr, sizeof(VHDR))) {
			if ((hdr.sign == 0x010000) && (hdr.flag == 0x0F)) {
				if (!file.GetPointer(&chunk_offset)) {
					break;
				}
				chunk_offset -= sizeof(VHDR); 
				if (hdr.type == 0x0D) {
					if (sizeof(DWORD) != file.Read(&size, sizeof(DWORD))) {
						break;
					}
					if (!file.SetPointer((LONGLONG)size, FILE_CURRENT)) {
						break;
					}
					continue;
				} else if (hdr.type == 0x0C) {					
					if (sizeof(DWORD) != file.Read(&unk, sizeof(DWORD))) {
						break;
					}
					if (sizeof(DWORD) != file.Read(&time, sizeof(DWORD))) {
						break;
					}					
					if (sizeof(DWORD) != file.Read(&size, sizeof(DWORD))) {
						break;
					}
					if (!file.SetPointer((LONGLONG)size, FILE_CURRENT)) {
						break;
					}
					continue;
				} else {
					// unknown type !!!
					int g = 0;
				}
			}
			int zz = 0;
		}
		int y = 0;
	}

}

} // namespace end