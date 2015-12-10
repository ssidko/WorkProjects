#include "sqliter.h"

namespace sqliter
{

int Sqliter_main() 
{
	return 0;
}

BYTE ReadInteger(BYTE *buff, DWORD serial_type, ULONGLONG &int_value)
{
	assert(buff);
	assert((serial_type >= k8BitInteger) && (serial_type <= k64BitInteger));

	switch (serial_type) {
	case k8BitInteger:
		int_value = *((BYTE *)buff);
		return 1;
	case k16BitInteger:
		int_value = (ULONGLONG)*((unsigned short *)buff);
		return 2;
	case k24BitInteger:
		if (*buff && (1 << 7)) {
			int_value = -1;
			((BYTE *)&int_value)[0] &= buff[2];
			((BYTE *)&int_value)[1] &= buff[1];
			((BYTE *)&int_value)[2] &= buff[0];
		}
		else {
			int_value = 0;
			((BYTE *)&int_value)[0] = buff[2];
			((BYTE *)&int_value)[1] = buff[1];
			((BYTE *)&int_value)[2] = buff[0];
		}
		return 3;
	case k32BitInteger:
		int_value = (ULONGLONG)(DWORD)Be2Le((DWORD *)buff);
		return 4;
	case k48BitInteger:
		if (*buff && (1 << 7)) {
			int_value = -1;
			((BYTE *)&int_value)[0] &= buff[5];
			((BYTE *)&int_value)[1] &= buff[4];
			((BYTE *)&int_value)[2] &= buff[3];
			((BYTE *)&int_value)[3] &= buff[2];
			((BYTE *)&int_value)[4] &= buff[1];
			((BYTE *)&int_value)[5] &= buff[0];
		}
		else {
			int_value = 0;
			((BYTE *)&int_value)[0] = buff[5];
			((BYTE *)&int_value)[1] = buff[4];
			((BYTE *)&int_value)[2] = buff[3];
			((BYTE *)&int_value)[3] = buff[2];
			((BYTE *)&int_value)[4] = buff[1];
			((BYTE *)&int_value)[5] = buff[0];
		}
		return 6;
	case k64BitInteger:
		((BYTE *)&int_value)[0] = buff[7];
		((BYTE *)&int_value)[1] = buff[6];
		((BYTE *)&int_value)[2] = buff[5];
		((BYTE *)&int_value)[3] = buff[4];
		((BYTE *)&int_value)[4] = buff[3];
		((BYTE *)&int_value)[5] = buff[2];
		((BYTE *)&int_value)[6] = buff[1];
		((BYTE *)&int_value)[7] = buff[0];
		return 8;
	default :
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//												class Record
//////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL Record::Initialize(BYTE *raw_record, DWORD record_size)
{
	assert(raw_record && record_size);

	Clean();

	DWORD len = 0;
	BYTE *next_field = raw_record;
	BYTE *next_value = 0;
	LONGLONG header_size = 0;

	next_field += GetVarint(raw_record, &header_size);

	if (header_size > record_size) {
		return false;
	}

	next_value = &raw_record[header_size];

	FIELD *field = NULL;
	LONGLONG serial_type = 0;

	while ( ((DWORD)(next_field - raw_record) < header_size) && (next_value < (raw_record + record_size)) ) {
		next_field += GetVarint(next_field, &serial_type);	

		if (serial_type == kNull) {
			field = new INTEGER_FIELD((LONGLONG)0);
			fields.push_back(field);
		} else if ((serial_type >= k8BitInteger) && (serial_type <= k64BitInteger)) {
			field = new INTEGER_FIELD();
			next_value += ReadInteger(next_value, (DWORD)serial_type, ((INTEGER_FIELD *)field)->val);
			fields.push_back(field);
		}else if (serial_type == k64BitFloat) {
			field = new FLOAT_FIELD();
			next_value += ReadInteger(next_value, k64BitInteger, ((INTEGER_FIELD *)field)->val);
			fields.push_back(field);
		} else if (serial_type == k0Constant) {
			field = new INTEGER_FIELD((LONGLONG)0);
			fields.push_back(field);
		} else if (serial_type == k1Constant) {
			field = new INTEGER_FIELD((LONGLONG)1);
			fields.push_back(field);
		} else if ( (serial_type >= kBlobMin) && ((serial_type % 2) == 0x00) ) {
			len = (DWORD)((serial_type - kBlobMin) / 2);
			field = new BLOB_FIELD();
			if ( len && ((next_value + len) <= (raw_record + record_size)) ) {
				((BLOB_FIELD *)field)->val.Initialize(next_value, len);
				next_value += len;
			}
			fields.push_back(field);
		} else if ( (serial_type >= kStringMin) && ((serial_type % 2) == 0x01) ) {
			len = (DWORD)((serial_type - kStringMin) / 2);
			field = new STRING_FIELD();
			if ( len && ((next_value + len) <= (raw_record + record_size)) ) {
				UTF8ToCP1251((const char *)next_value, len, &((STRING_FIELD *)field)->val);
				next_value += len;
			}
			fields.push_back(field);
		} else {
			return FALSE;
		}
	}
	return TRUE;
}

Record::~Record(void)
{
	Clean();
}

void Record::Clean(void)
{
	for (DWORD i = 0; i < fields.size(); i++) {
		delete fields[i];
	}
	fields.clear();
}

const FIELD *Record::operator[](DWORD idx)
{
	if (idx < fields.size()) {
		return fields[idx];
	}
	return FALSE;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////
//												class Page
//////////////////////////////////////////////////////////////////////////////////////////////////////

Page::Page(BYTE *page_buff, DWORD page_size, DWORD page_number) : buff(page_buff), size(page_size), number(page_number)
{
	Initialize();
}

void Page::Initialize(void)
{
	InitializeHeader();
	InitializeCellPointerArray();
}

void Page::InitializeHeader(void)
{
	PAGE_HEADER *header = (PAGE_HEADER *)buff;
	if (number == 0x01) {
		header = (PAGE_HEADER *)&buff[100];
	}
	memset(&hdr, 0x00, sizeof(PAGE_HEADER));
	if (IsValidPageType(header->type)) {
		hdr.type = header->type;
		hdr.first_freeblock = Be2Le(&header->first_freeblock);
		hdr.cells_count = Be2Le(&header->cells_count);
		hdr.cells_area = Be2Le(&header->cells_area);
		if ((hdr.type == kIntIndexPage) || (hdr.type == kIntTablePage)) {
			hdr.right_ptr = Be2Le(&header->right_ptr);
			hdr.offsets = (WORD *)&header->offsets;
		}
		else {
			hdr.offsets = (WORD *)&header->right_ptr;
		}
		if (!IsValidPage()) {
			memset(&hdr, 0x00, sizeof(PAGE_HEADER));
		}
	}
}

void Page::InitializeCellPointerArray()
{
	if (IsValidPageType(hdr.type)) {
		WORD *idx = hdr.offsets;
		for (DWORD i = 0; i < hdr.cells_count; i++) {
			idx[i] = Be2Le(&idx[i]);
			if (idx[i] >= size) {
				idx[i] = 0;
			}
		}
	}
}

BOOL Page::IsValidPageType(const BYTE &page_type)
{
	if (page_type == kLeafTablePage) {
		return TRUE;
	} else if (page_type == kLeafIndexPage) {
		return TRUE;
	} else if (page_type == kIntTablePage) {
		return TRUE;
	} else if (page_type == kIntIndexPage) {
		return TRUE;
	}
	return FALSE;
}

BOOL Page::IsValidPage(void)
{
	if (IsValidPageType(hdr.type)) {
		if (hdr.first_freeblock < size) {
			if (hdr.cells_area < size) {
				if (hdr.cells_count < size) {
					return true;
				}
			}			
		}	
	}
	return false;
}

void Page::Cleanup(void)
{
	if (buff) {
		delete[] buff;
		buff = NULL;
	}
	number = size = 0;
}

void Page::Initialize(BYTE *page_buff, DWORD page_size, DWORD page_number)
{
	assert (page_buff && page_size);

	Cleanup();
	buff = page_buff;
	size = page_size;
	number = page_number;
	Initialize();
}

BYTE *Page::GetCell(DWORD cell_num, DWORD *max_size)
{
	assert(hdr.cells_count && (cell_num < hdr.cells_count));

	WORD *idx = hdr.offsets;
	if ((idx[cell_num] > 0) && (idx[cell_num] < this->size)) {
		if (max_size) {
			*max_size = GetAvaliableBytesForCell(cell_num);
		}
		return &buff[idx[cell_num]];
	}
	return NULL;
}

DWORD Page::GetAvaliableBytesForCell(DWORD cell_num)
{
	assert(hdr.cells_count && (cell_num < hdr.cells_count));

	WORD *idx = hdr.offsets;
	WORD cell_offs = idx[cell_num];
	DWORD threshold = size;
	for (DWORD i = 0; i < hdr.cells_count; i++) {
		if ((idx[i] > cell_offs) && (idx[i] < threshold)) {
			threshold = idx[i];
		}
	}

	assert (threshold >= cell_offs);

	return (DWORD)(threshold - cell_offs);
}

DWORD Page::RecordsCount(void)
{
	if ((hdr.type == kIntIndexPage) || (hdr.type == kLeafIndexPage) || (hdr.type == kLeafTablePage)) {
		return hdr.cells_count;
	}
	return 0;
}

BOOL Page::GetRecord(DWORD record_num, Record *record)
{
	assert(hdr.cells_count && (record_num < hdr.cells_count) && record);
	if ((hdr.type == kIntIndexPage) || (hdr.type == kLeafIndexPage) || (hdr.type == kLeafTablePage)) {
		DWORD rw = 0;
		LONGLONG payload_size = 0;
		LONGLONG row_id = 0;
		DWORD max_cell_size = 0;
		BYTE *payload = NULL;
		BYTE *raw_cell = GetCell(record_num, &max_cell_size);
		if (raw_cell) {
			switch (hdr.type) {
				case kLeafTablePage:
					rw += GetVarint(&raw_cell[0], &payload_size);
					rw += GetVarint(&raw_cell[rw], &row_id);
					payload = &raw_cell[rw];
					if ( payload_size && ((payload_size + rw) <= (LONGLONG)max_cell_size) ) {
						return record->Initialize(payload, (DWORD)payload_size);
					}
					break;
				default :
					break;
			}
		}
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//												class SQLiter
//////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL SQLiter::ReadDbHeader(DB_HEADER *header)
{
	assert(io.IsOpen());

	io.SetPointer(0);
	if (sizeof(DB_HEADER) == io.Read(header, sizeof(DB_HEADER))) {
		if (0x00 == strcmp((const char *)header->magic_string, DB_HEADER_MAGIC_STRING)) {
			header->page_size = Be2Le(&header->page_size);
			header->change_counter = Be2Le(&header->change_counter);
			header->db_size = Be2Le(&header->db_size);
			header->freelist_page = Be2Le(&header->freelist_page);
			header->free_pages_counter = Be2Le(&header->free_pages_counter);
			header->schema_cookie = Be2Le(&header->schema_cookie);
			header->schema_format = Be2Le(&header->schema_format);
			header->default_cache_size = Be2Le(&header->default_cache_size);
			header->largest_root_page = Be2Le(&header->largest_root_page);
			header->text_encoding = Be2Le(&header->text_encoding);
			header->user_ver = Be2Le(&header->user_ver);
			header->incremental_vacuum_mode = Be2Le(&header->incremental_vacuum_mode);
			header->version_valid_for_number = Be2Le(&header->version_valid_for_number);
			header->sqlite_ver = Be2Le(&header->sqlite_ver);
			return TRUE;
		}
	}
	DWORD err = ::GetLastError();
	return FALSE;
}

void SQLiter::InitializeFreePagesList(void)
{
	if (free_pages) {
		delete [] free_pages;
		free_pages = NULL;
		free_pages_counter = 0;
	}

	DWORD res = 0;
	BYTE *page = new BYTE[hdr.page_size];
	DWORD next_page = hdr.freelist_page;
	DWORD pointers_per_page = 0;
	memset(page, 0x00, hdr.page_size);

	DWORD pages_counter = 0;
	free_pages = new DWORD[hdr.free_pages_counter];
	memset(free_pages, 0x00, hdr.free_pages_counter * sizeof(DWORD));

	do {
		res = (DWORD)io.SetPointer((LONGLONG)(next_page - 1) * hdr.page_size);
		assert(res);
		res = io.Read(page, hdr.page_size);
		assert(res);
		next_page = Be2Le((DWORD *)page);
		pointers_per_page = Be2Le((DWORD *)&page[sizeof(DWORD)]);

		assert(next_page <= hdr.db_size);
		assert(pointers_per_page <= (hdr.page_size / sizeof(DWORD)));
		assert(hdr.free_pages_counter >= (pages_counter + pointers_per_page));

		for (DWORD i = 0; i < pointers_per_page; i++) {
			free_pages[pages_counter + i] = Be2Le((DWORD *)&page[(2 + i) * sizeof(DWORD)]);
		}
		pages_counter += pointers_per_page;
	} while (next_page);

	free_pages_counter = pages_counter;
	delete[] page;
}

BOOL SQLiter::Open()
{
	Close();
	if (io.Open()) {
		memset(&hdr, 0x00, sizeof(DB_HEADER));
		if (ReadDbHeader(&hdr)) {
			if (hdr.free_pages_counter && hdr.freelist_page) {
				InitializeFreePagesList();
			}
			opened = TRUE;
			return TRUE;
		}
	}
	return FALSE;
}

void SQLiter::Close()
{
	if (free_pages) {
		delete [] free_pages;
		free_pages = NULL;
		free_pages_counter = 0;
	}

	if (io.IsOpen()) {
		io.Close();
	}

	opened = FALSE;
}

DWORD SQLiter::PagesCount(void)
{
	if (opened && hdr.page_size) {
		LONGLONG file_size = io.GetSize();
		return (DWORD)(file_size/hdr.page_size);
	}
	return 0;
}

BOOL SQLiter::ReadPage(DWORD page_num, BYTE *buff)
{
	assert(buff);

	if (opened) {
		if (page_num && (page_num <= PagesCount())) {
			if (io.SetPointer((LONGLONG)(page_num - 1)*hdr.page_size)) {
				return ((DWORD)hdr.page_size == io.Read(buff, (DWORD)hdr.page_size));
			}
		}
	}
	return FALSE;
}

Page *SQLiter::GetPage(DWORD page_num)
{
	if (opened) {
		BYTE *buff = new BYTE[hdr.page_size];
		if (ReadPage(page_num, buff)) {
			return new Page(buff, hdr.page_size, page_num);
		}
		else {
			delete[] buff;
		}
	}
	return NULL;
}

DWORD SQLiter::ReadFreePage(DWORD page_num, BYTE *buff)
{
	assert(opened);
	assert(buff);

	if (page_num <= free_pages_counter) {
		if (io.SetPointer((LONGLONG)(free_pages[page_num] - 1) * hdr.page_size)) {
			if (hdr.page_size == io.Read(buff, hdr.page_size)) {
				return free_pages[page_num];
			}
		}
	}
	return 0x00;
}

Page *SQLiter::GetFreePage(DWORD page_num)
{
	if (opened) {
		BYTE *buff = new BYTE[hdr.page_size];
		DWORD real_page_num = 0;
		if (real_page_num = ReadFreePage(page_num, buff)) {
			return new Page(buff, hdr.page_size, real_page_num);
		}
	}
	return NULL;
}

DWORD SQLiter::TestFunction(void *param)
{
	return 0;
}


#define SLOT_2_0     0x001fc07f
#define SLOT_4_2_0   0xf01fc07f

/*
** Read a 64-bit variable-length integer from memory starting at p[0].
** Return the number of bytes read.  The value is stored in *v.
*/
BYTE GetVarint(BYTE *p, LONGLONG *v)
{
	DWORD a,b,s;

	a = *p;
	/* a: p0 (unmasked) */
	if (!(a&0x80))
	{
		*v = a;
		return 1;
	}

	p++;
	b = *p;
	/* b: p1 (unmasked) */
	if (!(b&0x80))
	{
		a &= 0x7f;
		a = a<<7;
		a |= b;
		*v = a;
		return 2;
	}

	/* Verify that constants are precomputed correctly */
	assert( SLOT_2_0 == ((0x7f<<14) | (0x7f)) );
	assert( SLOT_4_2_0 == ((0xfU<<28) | (0x7f<<14) | (0x7f)) );

	p++;
	a = a<<14;
	a |= *p;
	/* a: p0<<14 | p2 (unmasked) */
	if (!(a&0x80))
	{
		a &= SLOT_2_0;
		b &= 0x7f;
		b = b<<7;
		a |= b;
		*v = a;
		return 3;
	}

	/* CSE1 from below */
	a &= SLOT_2_0;
	p++;
	b = b<<14;
	b |= *p;
	/* b: p1<<14 | p3 (unmasked) */
	if (!(b&0x80))
	{
		b &= SLOT_2_0;
		/* moved CSE1 up */
		/* a &= (0x7f<<14)|(0x7f); */
		a = a<<7;
		a |= b;
		*v = a;
		return 4;
	}

	/* a: p0<<14 | p2 (masked) */
	/* b: p1<<14 | p3 (unmasked) */
	/* 1:save off p0<<21 | p1<<14 | p2<<7 | p3 (masked) */
	/* moved CSE1 up */
	/* a &= (0x7f<<14)|(0x7f); */
	b &= SLOT_2_0;
	s = a;
	/* s: p0<<14 | p2 (masked) */

	p++;
	a = a<<14;
	a |= *p;
	/* a: p0<<28 | p2<<14 | p4 (unmasked) */
	if (!(a&0x80))
	{
		/* we can skip these cause they were (effectively) done above in calc'ing s */
		/* a &= (0x7f<<28)|(0x7f<<14)|(0x7f); */
		/* b &= (0x7f<<14)|(0x7f); */
		b = b<<7;
		a |= b;
		s = s>>18;
		*v = ((LONGLONG)s)<<32 | a;
		return 5;
	}

	/* 2:save off p0<<21 | p1<<14 | p2<<7 | p3 (masked) */
	s = s<<7;
	s |= b;
	/* s: p0<<21 | p1<<14 | p2<<7 | p3 (masked) */

	p++;
	b = b<<14;
	b |= *p;
	/* b: p1<<28 | p3<<14 | p5 (unmasked) */
	if (!(b&0x80))
	{
		/* we can skip this cause it was (effectively) done above in calc'ing s */
		/* b &= (0x7f<<28)|(0x7f<<14)|(0x7f); */
		a &= SLOT_2_0;
		a = a<<7;
		a |= b;
		s = s>>18;
		*v = ((LONGLONG)s)<<32 | a;
		return 6;
	}

	p++;
	a = a<<14;
	a |= *p;
	/* a: p2<<28 | p4<<14 | p6 (unmasked) */
	if (!(a&0x80))
	{
		a &= SLOT_4_2_0;
		b &= SLOT_2_0;
		b = b<<7;
		a |= b;
		s = s>>11;
		*v = ((LONGLONG)s)<<32 | a;
		return 7;
	}

	/* CSE2 from below */
	a &= SLOT_2_0;
	p++;
	b = b<<14;
	b |= *p;
	/* b: p3<<28 | p5<<14 | p7 (unmasked) */
	if (!(b&0x80))
	{
		b &= SLOT_4_2_0;
		/* moved CSE2 up */
		/* a &= (0x7f<<14)|(0x7f); */
		a = a<<7;
		a |= b;
		s = s>>4;
		*v = ((LONGLONG)s)<<32 | a;
		return 8;
	}

	p++;
	a = a<<15;
	a |= *p;
	/* a: p4<<29 | p6<<15 | p8 (unmasked) */

	/* moved CSE2 up */
	/* a &= (0x7f<<29)|(0x7f<<15)|(0xff); */
	b &= SLOT_2_0;
	b = b<<8;
	a |= b;

	s = s<<4;
	b = p[-4];
	b &= 0x7f;
	b = b>>3;
	s |= b;

	*v = ((LONGLONG)s)<<32 | a;

	return 9;
}

string UTF8ToCP1251(const char *str)
{
	string res;    
	int result_u, result_c;

	result_u = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
	if (!result_u) {
		return 0;
	}

	wchar_t *ures = new wchar_t[result_u];
	if(!MultiByteToWideChar(CP_UTF8, 0, str, -1, ures, result_u)) {
		delete[] ures;
		return 0;
	}

	result_c = WideCharToMultiByte(1251, 0, ures, -1, 0, 0, 0, 0);
	if(!result_c) {
		delete [] ures;
		return 0;
	}

	char *cres = new char[result_c];
	if(!WideCharToMultiByte(1251, 0, ures, -1, cres, result_c, 0, 0)) {
		delete[] cres;
		return 0;
	}

	delete[] ures;
	res.append(cres);
	delete[] cres;
	
	return res;
}

BOOL UTF8ToCP1251(const char *str, DWORD len, string *res_str)
{
	assert(str && len && res_str);
 
	int result_u, result_c;

	result_u = MultiByteToWideChar(CP_UTF8, 0, str, len, 0, 0);
	if (!result_u) {
		return FALSE;
	}

	wchar_t *ures = new wchar_t[result_u + 1];
	memset(ures, 0x00, sizeof(wchar_t)*(result_u + 1));
	if(!MultiByteToWideChar(CP_UTF8, 0, str, len, ures, result_u)) {
		delete[] ures;
		return FALSE;
	}

	result_c = WideCharToMultiByte(1251, 0, ures, -1, 0, 0, 0, 0);
	if(!result_c) {
		delete [] ures;
		return FALSE;
	}

	char *cres = new char[result_c];
	if(!WideCharToMultiByte(1251, 0, ures, -1, cres, result_c, 0, 0)) {
		delete[] cres;
		return FALSE;
	}

	delete[] ures;
	res_str->assign(cres);
	delete[] cres;

	return TRUE;

}

void testing_recovery(void) {

#define CHAT_NAME_FIELD				(DWORD)3
#define AUTHOR_FIELD				(DWORD)4
#define DIALOG_PARTNER_FIELD		(DWORD)8
#define TIMESTAMP_FIELD				(DWORD)9
#define MESSAGE_TYPE_FIELD			(DWORD)10
#define MESSAGE_TEXT_FIELD			(DWORD)17

	enum MessageType {
		kAddToContactsList = 50,
		kText = 61,
		kSendFile = 68
	};

	//static const DWORD max_len = 2048;
	//char line[max_len] = {0};
	//char timestamp_str[max_len] = {0};
	char time_string[32];

	FileEx out_file(_T("J:\\Work\\33116\\out.txt"));

	Record rec;
	const FIELD *field = NULL;
	const STRING_FIELD *str_field = NULL;
	const INTEGER_FIELD *int_field = NULL;

	multimap<time_t, string> messages;

	SQLiter db(_T("J:\\Work\\33116\\main.db"));
	//SQLiter db(_T("d:\\GitHub\\main.db"));
	if (db.Open() && out_file.Create()) {
		for (DWORD i = 1; i <= db.FreePagesCount(); i++) {
			Page *page = db.GetFreePage(i);

			//for (DWORD i = 1; i <= db.PagesCount(); i++) {
			//	Page *page = db.GetPage(i);

			if (page && (page->Type() == kLeafTablePage)) {
				for (DWORD r = 0; r < page->RecordsCount(); r++) {
					if (page->GetRecord(r, &rec)) {

						string out_line = "";
						string chat_name = "";
						string message_text = "";
						string author = "";
						string dialog_partner;
						string time = "";
						time_t raw_time = 0;

						if (rec.FieldsCount() != 36) continue;

						if (rec[CHAT_NAME_FIELD]->type != kString) continue;
						chat_name = ((STRING_FIELD *)rec[CHAT_NAME_FIELD])->val;

						if ((chat_name.find("oleg_anshlag") == string::npos) || (chat_name.find("babay.andr") == string::npos)) continue;

						if (((INTEGER_FIELD *)rec[MESSAGE_TYPE_FIELD])->val != kText) continue;

						if (rec[MESSAGE_TEXT_FIELD]->type != kString) continue;
						message_text = ((STRING_FIELD *)rec[MESSAGE_TEXT_FIELD])->val;

						if (rec[AUTHOR_FIELD]->type != kString) continue;
						author = ((STRING_FIELD *)rec[AUTHOR_FIELD])->val;

						if (rec[DIALOG_PARTNER_FIELD]->type != kString) continue;
						dialog_partner = ((STRING_FIELD *)rec[DIALOG_PARTNER_FIELD])->val;

						if (rec[TIMESTAMP_FIELD]->type != kInteger) continue;
						raw_time = (time_t)((INTEGER_FIELD *)rec[TIMESTAMP_FIELD])->val;
						memset(time_string, 0x00, sizeof(time_string));
						ctime_s(time_string, sizeof(time_string), &raw_time);
						time = time_string;

						time[time.size() - 1] = ' ';

						out_line += time;
						out_line += "- ";
						out_line += author;

						//out_line += "(";
						//if (author.find(dialog_partner) == string::npos) {
						//	out_line += dialog_partner;
						//} else {
						//	out_line += "oleg_anshlag";
						//}
						//out_line += ")";

						out_line += ": ";
						out_line += message_text;
						out_line += "\r\n";

						messages.insert (pair<time_t, string>(raw_time, out_line));

						//out_file.Write((void *)out_line.c_str(), out_line.size());
						int x = 0;
					}
				}
				delete page;
				page = NULL;
			}
		}
	}

	multimap<time_t, string>::iterator it;
	for (it = messages.begin(); it != messages.end(); ++it) {
		out_file.Write((void *)((*it).second.c_str()), (*it).second.size());
	}
}

}

