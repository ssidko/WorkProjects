#include "DBFile.h"

DBFile::DBFile(const char *file_name) : io(NULL)
{
	int x = 0;
	io = new std::fstream(file_name, std::ios_base::binary|std::ios_base::in|std::ios_base::out);

}


DBFile::~DBFile(void)
{
}

bool DBFile::Open( void )
{
	if (io && io->is_open()) {
		HEADER header;
		memset(&header, 0x00, sizeof(HEADER));

		io->read((char *)&header, sizeof(HEADER));
		int x = header.header_size + header.record_size*header.records_count + 1;
		x++;
	}
	return false;
}
