#pragma once

#include <fstream>

class DBFile
{
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
		unsigned char index_flag;
		unsigned char reserved_3[3];
	} HEADER;

	typedef struct _FIELD_DESCRIPTOR {
		unsigned char name[11];
		unsigned char type;				// 'C', 'N', 'D'.
		unsigned char reserved_1[4];	// 0x00
		unsigned char size;
		unsigned char fraction_size;	// Кол-во знаков после запятой
		unsigned char reserved_2[14];	// 0x00
	} FIELD_DESCRIPTOR;

#pragma pack(pop)

private:
	std::fstream *io;

public:
	unsigned int header_size;
	unsigned int record_size;
	unsigned int records_count;

	DBFile(const char *file_name);
	~DBFile(void);

	bool ReadHeader(HEADER *header);
	bool IsValidHeader(HEADER *header);
	bool GetRecord(unsigned int record_number, char *record);
	bool WriteRecord(unsigned int record_number, char *record);
	bool IsValidRecord(unsigned char *record);
	void RepairBadRecords(void);

	bool Open(void);
};

