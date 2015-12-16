#pragma once
#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include "BufferedFile.h"

typedef DWORD(WINAPI *FOREACHFILEPROC)(std::string &path);

BOOL ForEachFileFolder(const char *path,
	FOREACHFILEPROC file_proc,
	FOREACHFILEPROC folder_proc = NULL,
	BOOL check_subfolders = FALSE);

#pragma pack (push)
#pragma pack (1)

typedef struct _IndexRecord {
	DWORD file_id;
	BYTE camera;
	BYTE unk_1[3];
	DWORD start_time;
	DWORD end_time;
	_IndexRecord() { memset(this, 0x00, sizeof(_IndexRecord)); }
} IndexRecord;

#pragma pack(pop)


namespace dvr1
{
	int Run(const char *raw_files_path, const char *out_dir_path, const char *index_file_name);
	bool InitializeIndexList(const std::string  &index_file_name, std::vector<IndexRecord> &idx_list);
	void FileName(IndexRecord &recort, std::string &file_name);
	void ConvertFile(IndexRecord &idx_record, std::string &raw_file_path, const std::string &out_dir);
	void TimeToString(DWORD time, std::string &str);
};