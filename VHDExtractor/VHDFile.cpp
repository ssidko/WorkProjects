#include "VHDFile.h"
#include <assert.h>

VHDFile::VHDFile(QString file_name)
{
	io = new QFile(file_name);
}

VHDFile::~VHDFile()
{
}

bool VHDFile::Open()
{
	return io->open(QIODevice::ReadOnly);
}

bool VHDFile::ReadFooter(void)
{
	assert(io);
	VHD_FOOTER *footer = NULL;
	char buff[VHD_SECTOR_SIZE] = {0};

	if (!io->seek(io->size() - 512)) return false;
	if (io->read(buff, VHD_SECTOR_SIZE) == -1) return false;

	footer = (VHD_FOOTER *)buff;
	if (QByteArray(footer->cookie, 8) != QByteArray("conectix", 8)) return false;

	return true;
}
