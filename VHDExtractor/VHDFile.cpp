#include "VHDFile.h"
#include <assert.h>

WORD Be2Le(WORD be_w)
{
	WORD le_w;
	BYTE *le = (BYTE *)&le_w;
	BYTE *be = (BYTE *)&be_w;
	le[0] = be[1];
	le[1] = be[0];
	return le_w;
}

DWORD Be2Le(DWORD be_dw)
{
	DWORD le_dw;
	BYTE *le = (BYTE *)&le_dw;
	BYTE *be = (BYTE *)&be_dw;
	le[0] = be[3];
	le[1] = be[2];
	le[2] = be[1];
	le[3] = be[0];
	return le_dw;
}

LONGLONG Be2Le(LONGLONG be_ll)
{
	LONGLONG le_ll;
	BYTE *le = (BYTE *)&le_ll;
	BYTE *be = (BYTE *)&be_ll;
	le[0] = be[7];
	le[1] = be[6];
	le[2] = be[5];
	le[3] = be[4];
	le[4] = be[3];
	le[5] = be[2];
	le[6] = be[1];
	le[7] = be[0];
	return le_ll;
}

VHDFile::VHDFile(QString file_name)
{
	io = new QFile(file_name);
	memset(&footer, 0x00, sizeof(VHD_FOOTER));
	memset(&dd_header, 0x00, sizeof(VHD_DYNAMIC_DISK_HEADER));
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

	memset(&footer, 0x00, sizeof(VHD_FOOTER));

	if (!io->seek(io->size() - sizeof(VHD_FOOTER))) return false;
	if (io->read((char *)&footer, sizeof(VHD_FOOTER)) == -1) return false;

	if (QByteArray(footer.cookie, 8) == QByteArray("conectix", 8)) {
		footer.features = Be2Le(footer.features);
		footer.version = Be2Le(footer.version);
		footer.data_offset = Be2Le(footer.data_offset);
		footer.time_stamp = Be2Le(footer.time_stamp);
		footer.creator_ver = Be2Le(footer.creator_ver);
		footer.original_size = Be2Le(footer.original_size);
		footer.current_size = Be2Le(footer.current_size);
		footer.geometry.cylinder = Be2Le(footer.geometry.cylinder);
		footer.disk_type = Be2Le(footer.disk_type);
		footer.checksum = Be2Le(footer.checksum);
		return true;
	} else {
		return false;
	}
}

bool VHDFile::ReadDynamicDiskHeader(void)
{
	assert(io);

	memset(&dd_header, 0x00, sizeof(VHD_DYNAMIC_DISK_HEADER));

	if (!io->seek(footer.data_offset)) return false;
	if (io->read((char *)&dd_header, sizeof(VHD_DYNAMIC_DISK_HEADER)) == -1) return false;

	if (QByteArray(dd_header.cookie, 8) == QByteArray("cxsparse", 8)) {
		dd_header.data_offset = Be2Le(dd_header.data_offset);
		dd_header.table_offset = Be2Le(dd_header.table_offset);
		dd_header.header_version = Be2Le(dd_header.header_version);
		dd_header.max_table_entries = Be2Le(dd_header.max_table_entries);
		dd_header.block_size = Be2Le(dd_header.block_size);
		dd_header.checksum = Be2Le(dd_header.checksum);
		dd_header.parent_time = Be2Le(dd_header.parent_time);
		for (int i = 0; i < 8; i++) {
			dd_header.entries[i].platform_code = Be2Le(dd_header.entries[i].platform_code);
			dd_header.entries[i].platform_data_space = Be2Le(dd_header.entries[i].platform_data_space);
			dd_header.entries[i].platform_data_length = Be2Le(dd_header.entries[i].platform_data_length);
			dd_header.entries[i].platform_data_offset = Be2Le(dd_header.entries[i].platform_data_offset);
		}
		return true;
	} else {
		return false;
	}
}
