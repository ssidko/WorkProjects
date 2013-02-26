#include "sqliter.h"

int Sqliter_main() 
{
	return 0;
}

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
	return FALSE;
}

void SQLiter::InitializeFreePagesList()
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
		int x = 0;
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
			InitializeFreePagesList();
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

DWORD SQLiter::ReadFreePage(DWORD page_num, BYTE *buff)
{
	assert(opened);
	assert(buff);
	assert(page_num < free_pages_counter);

	if (io.SetPointer((LONGLONG)(free_pages[page_num] - 1) * hdr.page_size)) {
		if (hdr.page_size == io.Read(buff, hdr.page_size)) {
			return free_pages[page_num];
		}
	}
	return 0x00;
}

DWORD SQLiter::TestFunction(void *param)
{
	DWORD page_count = GetFreePagesCount();
	BYTE page[4096];
	PAGE_HEADER *page_hdr = (PAGE_HEADER *)page;
	DWORD cell_offset = 0;

	for (DWORD i = 0; i < page_count; i++) {
		ReadFreePage(i, page);
		if (page_hdr->type == 0x0D) {
			int z = 0;

		}

		int y = 0;
	}
	int x = 0;
	return 0;
}


#define SLOT_2_0     0x001fc07f
#define SLOT_4_2_0   0xf01fc07f

/*
** Read a 64-bit variable-length integer from memory starting at p[0].
** Return the number of bytes read.  The value is stored in *v.
*/
BYTE GetVarint(BYTE *p, ULONGLONG *v)
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
		*v = ((ULONGLONG)s)<<32 | a;
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
		*v = ((ULONGLONG)s)<<32 | a;
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
		*v = ((ULONGLONG)s)<<32 | a;
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
		*v = ((ULONGLONG)s)<<32 | a;
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

	*v = ((ULONGLONG)s)<<32 | a;

	return 9;
}

