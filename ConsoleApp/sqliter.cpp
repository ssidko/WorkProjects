#include "sqliter.h"

int Sqliter_main() 
{
	return 0;
}

#pragma pack(push)
#pragma pack(1)

typedef struct _SQLITE_DB_HEADER {
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

} SQLITE_DB_HEADER;

typedef struct _SQLITE_PAGE_HEADER {
	BYTE type;
	WORD freeblock_offs;
	WORD cells_num;
	WORD cells_offs;
	BYTE number_fragmented_free_bytes;
	DWORD right_most_pointer;
} SQLITE_PAGE_HEADER;

#pragma pack(pop)

BOOL SQLiter::Open()
{
	if (io.Open()) {


	}
	return FALSE;
}

void SQLiter::Close()
{
	io.Close();
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

