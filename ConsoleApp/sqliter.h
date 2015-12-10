#pragma once
#include <Windows.h>
#include <assert.h>
#include <memory>
#include <vector>
#include "W32Lib.h"
#include <ctime>

namespace sqliter
{
	int Sqliter_main();
	BYTE ReadInteger(BYTE *buff, DWORD serial_type, LONGLONG &value);
	BYTE GetVarint(BYTE *p, LONGLONG *v);
	string UTF8ToCP1251(const char *str);
	BOOL UTF8ToCP1251(const char *str, DWORD len, string *res_str);

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
		DWORD text_encoding; // 1=UTF-8 2=UTF16le 3=UTF16be
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
		WORD cells_area;
		BYTE fragmented_bytes;
		DWORD right_ptr;
		WORD *offsets;
	} PAGE_HEADER;

#pragma pack(pop)

	typedef struct _CELL {
		BOOL is_overflow;
		DWORD left_child;
		LONGLONG payload_size;
		LONGLONG row_id;
		BYTE *payload;
		DWORD first_overflow;
	} CELL;

	enum FieldType {
		kInteger,
		kFloat,
		kBlob,
		kString,
	};

	enum FieldSerialType {
		kNull = 0,
		k8BitInteger,
		k16BitInteger,
		k24BitInteger,
		k32BitInteger,
		k48BitInteger,
		k64BitInteger,
		k64BitFloat,
		k0Constant,
		k1Constant,
		kBlobMin = 12,
		kStringMin = 13
	};

	class Blob
	{
	private:
		BYTE *buff;
		DWORD size;

	public:
		Blob() : buff(NULL), size(0) {}
		Blob(const BYTE *src_buff, DWORD buff_size) : buff(NULL), size(0)
		{
			Initialize(src_buff, buff_size);
		}
		void Initialize(const BYTE *src_buff, DWORD buff_size)
		{
			Clean();
			assert(src_buff);
			assert(buff_size);
			buff = new BYTE[buff_size];
			assert(buff);
			memcpy(buff, src_buff, buff_size);
			size = buff_size;
		}
		void Clean(void)
		{
			if (buff) {
				delete [] buff;
				buff = NULL;
				size = 0;
			}
		}
		~Blob() {Clean();}
		const BYTE *GetBuff(void) {return buff;}
	};

	typedef struct _FIELD {
		DWORD type;
		_FIELD (DWORD field_type) : type(field_type) {}
		virtual ~_FIELD() {}
	} FIELD;

	template<int field_type, typename T>
	struct TEMPLATED_FIELD : public FIELD {
		T val;
		TEMPLATED_FIELD () : FIELD(field_type) {}
		TEMPLATED_FIELD (T init_val) : FIELD(field_type), val(init_val) {}
	};

	typedef TEMPLATED_FIELD<kInteger, ULONGLONG> INTEGER_FIELD;
	typedef TEMPLATED_FIELD<kFloat, double> FLOAT_FIELD;
	typedef TEMPLATED_FIELD<kBlob, Blob> BLOB_FIELD;
	typedef TEMPLATED_FIELD<kString, string> STRING_FIELD;

	class Record
	{
	private:
		vector<FIELD *> fields;
		void Clean(void);
	public:
		Record() {}
		~Record();
		BOOL Initialize(BYTE *raw_record, DWORD record_size);
		DWORD FieldsCount(void) {return fields.size();}
		const FIELD *operator[](DWORD idx);
	};

	enum PageTypeFlag {
		kIntKey	= 0x01,
		kZeroData = 0x02,
		kLeafData = 0x04,
		kLeaf = 0x08
	};

	enum PageType {
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
		DWORD number;
		PAGE_HEADER hdr;
		void Initialize(void);
		void InitializeHeader(void);
		void InitializeCellPointerArray(void);
		BOOL IsValidPageType(const BYTE &page_type);
		BOOL IsValidPage(void);
		void Cleanup(void);
	public:
		Page(BYTE *page_buff, DWORD page_size, DWORD page_number);
		~Page()	{Cleanup();}
		void Initialize(BYTE *page_buff, DWORD page_size, DWORD page_number);
		DWORD Number(void) {return number;}
		DWORD Type(void) {return hdr.type;}
		DWORD CellsCount(void) {return hdr.cells_count;}
		DWORD RecordsCount(void);
		// нумерация с нуля.
		BYTE *GetCell(DWORD cell_num, DWORD *max_size);
		// нумерация с нуля.
		DWORD GetAvaliableBytesForCell(DWORD cell_num);
		// нумерация с нуля.
		BOOL GetRecord(DWORD record_num, Record *record);
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
		// нумерация с 1
		BOOL ReadPage(DWORD page_num, BYTE *buff);
		// нумерация с 1
		Page *GetPage(DWORD page_num);
		// В случае успеха возвращает номер страници (нумерация с 1), иначе 0x00.
		DWORD ReadFreePage(DWORD page_num, BYTE *buff);
		// нумерация с нуля.
		Page *GetFreePage(DWORD page_num);
		DWORD TestFunction(void *param);
	};

}
