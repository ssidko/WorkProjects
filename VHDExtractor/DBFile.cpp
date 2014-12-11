#include "DBFile.h"
#include <assert.h>
#include "ValidSymbols.h"

DBFile::DBFile(const char *file_name) : io(NULL)
{
	io = new std::fstream(file_name, std::ios_base::binary | std::ios_base::in | std::ios_base::out);
	assert(io);
}


DBFile::~DBFile(void)
{
	if (io && io->is_open()) {
		io->close();
	}
}

bool DBFile::Open( void )
{
	assert(io);
	if (io->is_open()) {
		HEADER header;
		memset(&header, 0x00, sizeof(HEADER));

		if (!ReadHeader(&header)) {
			return false;
		}

		if (!IsValidHeader(&header)) {
			return false;
		}

		header_size = header.header_size;
		record_size = header.record_size;
		records_count = header.records_count;

		int calculated_size = header.header_size + header.record_size*header.records_count + 1;
		return true;
	}
	return false;
}

bool DBFile::ReadHeader(HEADER *header)
{
	assert(io);
	assert(header);
	if (io->is_open()) {
		io->seekg(0);
		io->read((char *)header, sizeof(HEADER));
		if (io->gcount() == sizeof(HEADER)) {
			return true;
		}
	}
	return false;
}

bool DBFile::IsValidHeader( HEADER *header )
{
	assert(header);

	if (header->flags != 0x03) {
		return false;
	}

	// HEADER::reserved_2 and HEADER::reserved_3 must be 0x00
	for (int i = 0; i < sizeof(header->reserved_2); ++i) {
		if (header->reserved_2[i] != 0x00) {
			return false;
		}
	}
	for (int i = 0; i < sizeof(header->reserved_3); ++i) {
		if (header->reserved_3[i] != 0x00) {
			return false;
		}
	}

	return true;
}

bool DBFile::GetRecord(unsigned int record_number, char *record)
{
	assert(io);
	assert(io->is_open());

	io->seekg(header_size + record_number*record_size);
	io->read(record, record_size);
	if (io->gcount() == record_size) {
		return true;
	}
	return false;
}

bool DBFile::WriteRecord(unsigned int record_number, char *record)
{
	assert(io);
	assert(io->is_open());

	io->seekg(header_size + record_number*record_size);
	io->write(record, record_size);
	
	return true;
}

bool DBFile::IsValidRecord(unsigned char *record)
{
	assert(record);
	int ch = 0;
	for (int i = 0; i < record_size; ++i) {
		ch = record[i];
		if (valid_symbols_table[ch] == 0x00) {
			return false;
		}
	}
	return true;
}

void DBFile::RepairBadRecords( void )
{
	assert(io);
	assert(io->is_open());

	unsigned char *record = new unsigned char[record_size];
	unsigned char *def_record = new unsigned char[record_size];
	memset(def_record, 0x20, record_size);
	for (unsigned int i = 0; i < records_count; ++i ) {
		memset(record, 0x00, record_size);
		if (GetRecord(i, (char *)record)) {
			if (!IsValidRecord(record)) {
				WriteRecord(i, (char *)def_record);
			}
		}
	}

}
