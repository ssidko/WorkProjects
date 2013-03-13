#pragma once
#include <Windows.h>
#include <assert.h>
#include "W32Lib.h"

BYTE GetVarint(BYTE *p, ULONGLONG *v);
int Sqliter_main();


class SQLiter
{
#define DB_HEADER_MAGIC_STRING				"SQLite format 3"

#pragma pack(push)
#pragma pack(1)

	typedef struct _DB_HEADER {
		BYTE magic_string[16];
		WORD page_size;
		BYTE write_ver;
		BYTE read_ver;
		BYTE reserved_space_size;
		BYTE max_payload_fraction;
		BYTE min_payload_fraction;
		BYTE leaf_payload_fraction;
		DWORD change_counter;
		DWORD db_size;
		DWORD freelist_page;
		DWORD free_pages_counter;
		DWORD schema_cookie;
		DWORD schema_format;
		DWORD default_cache_size;
		DWORD largest_root_page;
		DWORD text_encoding;
		DWORD user_ver;
		DWORD incremental_vacuum_mode;
		BYTE reserved[24];
		DWORD version_valid_for_number;
		DWORD sqlite_ver;
	} DB_HEADER;

	typedef struct _PAGE_HEADER {
		BYTE type;
		WORD freeblock_offs;
		WORD cells_num;
		WORD cells_offs;
		BYTE number_fragmented_free_bytes;
		DWORD right_most_pointer;
	} PAGE_HEADER;

	typedef struct _TABLE_LEAF_CELL {
		LONGLONG payload_size;
		LONGLONG row_id;
		BYTE *paylod;
		DWORD overflow_page;
	} TABLE_LEAF_CELL;

#pragma pack(pop)

private:
	BOOL opened;
	FileEx io;
	DB_HEADER hdr;
	DWORD *free_pages;
	DWORD free_pages_counter;

	BOOL ReadDbHeader(DB_HEADER *header);
	void InitializeFreePagesList();

public:
	SQLiter(TCHAR *file_name) : opened(FALSE), io(file_name), free_pages(NULL), free_pages_counter(0) {}
	~SQLiter() {Close();}

	BOOL Open();
	void Close();

	DWORD GetPageSize() {assert(opened); return (DWORD)hdr.page_size;}
	DWORD GetDbSize() {assert(opened); return hdr.db_size;}
	DWORD GetFreePagesCount() {assert(opened); return free_pages_counter;}

	// Return size in pages
	DWORD Size(void);

	BOOL ReadPage(DWORD page_num, BYTE *buff);
	// В случае успеха возвращает номер страници (нумерация с 1), иначе 0x00.
	DWORD ReadFreePage(DWORD page_num, BYTE *buff);

	DWORD TestFunction(void *param);
};



