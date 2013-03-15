#pragma once
#include <Windows.h>
#include <assert.h>
#include "W32Lib.h"

namespace sqliter
{

	BYTE GetVarint(BYTE *p, ULONGLONG *v);
	int Sqliter_main();

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
		WORD first_freeblock;
		WORD cells_count;
		WORD cells_offs;
		BYTE fragmented_bytes;
		DWORD right_ptr;
	} PAGE_HEADER;

#pragma pack(pop)

	typedef struct _TABLE_LEAF_CELL {
		ULONGLONG payload_size;
		ULONGLONG row_id;
		BYTE *paylod;
		DWORD overflow_page;
	} TABLE_LEAF_CELL;

	//
	//  RECORD:
	//	- header
	//	--- header size
	//	--- columns
	//	--- . . . 
	//	- body		
	//

	enum {
		kIntIndexPage = 2,
		kIntTablePage = 5,
		kLeafIndexPage = 10,
		kLeafTablePage = 13
	};

	class Page 
	{
	private:
		BYTE *buff;
		DWORD size;
		PAGE_HEADER hdr;
		void InitializeHeader(void);
	public:
		Page(BYTE *page_buff, DWORD buff_size);
		~Page() {delete[] buff;}

		BOOL Initialize(void);
		DWORD Type(void) {return hdr.type;}
		DWORD CellsCount(void) {return hdr.cells_count;}
	};


	class SQLiter
	{
	private:
		BOOL opened;
		FileEx io;
		DB_HEADER hdr;
		DWORD *free_pages;
		DWORD free_pages_counter;
		BOOL ReadDbHeader(DB_HEADER *header);
		void InitializeFreePagesList(void);
	public:
		SQLiter(TCHAR *file_name) : opened(FALSE), io(file_name), free_pages(NULL), free_pages_counter(0) {}
		~SQLiter(void) {Close();}
		BOOL Open(void);
		void Close(void);
		DWORD PageSize(void) {assert(opened); return (DWORD)hdr.page_size;}
		DWORD PagesCount(void);
		DWORD FreePagesCount(void) {assert(opened); return free_pages_counter;}		
		BOOL ReadPage(DWORD page_num, BYTE *buff);
		// В случае успеха возвращает номер страници (нумерация с 1), иначе 0x00.
		DWORD ReadFreePage(DWORD page_num, BYTE *buff);
		DWORD TestFunction(void *param);
	};

}



