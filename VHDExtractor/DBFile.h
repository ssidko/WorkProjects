#pragma once

#include <fstream>

class DBFile
{
private:
	std::fstream *io;

public:

	#pragma pack(push)
	#pragma pack(1)

	typedef struct _HEADER {
		unsigned char flags;
		unsigned char year;				// 1900 + HEADER::year
		unsigned char month;
		unsigned char day;
		unsigned int records_count;
		unsigned short header_size;
		unsigned short record_size;
		unsigned short reserved_1;		// 0x00
		unsigned char transaction_incomplete_flag;
		unsigned char encryption_flag;
		unsigned char reserved_2[12];
		unsigned char mdx_flag;
		unsigned char reserved_3[3];



	} HEADER;

	#pragma pack(pop)

	unsigned int header_size;
	unsigned int record_size;
	unsigned int records_count;

	DBFile(const char *file_name);
	~DBFile(void);

	bool Open(void);
};

