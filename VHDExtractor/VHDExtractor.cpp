#include "VHDExtractor.h"
#include <QRegExp>

VHDExtractor::VHDExtractor(QString vhd_file, QString out_file)
{
	vhd_file_name = vhd_file.trimmed();
	out_file_name = out_file.trimmed();
}

VHDExtractor::~VHDExtractor(void)
{
}

void VHDExtractor::run()
{
	VHDFile vhd(vhd_file_name);
	if (!vhd.Open()) {
		emit(Error(QString::fromLocal8Bit("Could not open VHD file: \n") + vhd_file_name));
		emit(Finished(-1));
		return;
	}
	QRegExp disk_regexp("\\\\\\\\.\\\\PhysicalDrive\\d{1,3}");
	if (disk_regexp.indexIn(out_file_name) >= 0) {
		PhysicalDrive disk(out_file_name.toLocal8Bit().data());
		if (!disk.Open()) {
			emit(Error(QString::fromLocal8Bit("Could not open disk: \n") + out_file_name));
			emit(Finished(-1));
			return;
		} else {
			ExtractToDisk(vhd, disk);
		}
	} else {
		QFile out_file(out_file_name);
		if (!out_file.open(QIODevice::ReadWrite)) {
			emit(Error(QString::fromLocal8Bit("Could not open out file: \n") + out_file_name));
			emit(Finished(-1));
			return;
		} else {
			ExtractToFile(vhd, out_file);
		}
	}
	emit(Finished(0));
}

void VHDExtractor::ExtractToDisk(VHDFile &vhd, PhysicalDrive &disk)
{
	DWORD block_size = vhd.BlockSize();
	DWORD block_count = vhd.BlocksCount();
	char *buff = new char[block_size];
	if (!buff) {
		emit(Error(QString::fromLocal8Bit("Could not allocate buffer: ") + QString::number(block_size, 10)));
		emit(Finished(-1));
		return;
	}
	memset(buff, 0x00, block_size);
	for (DWORD i = 0; i < block_count; i++) {
		if (vhd.ReadBlock(i, buff)) {
			if (disk.Write(buff, block_size) < block_size) {
				emit(Error(QString::fromLocal8Bit("Could not write block: ") + QString::number(i, 10)));
				emit(Finished(-1));
				return;
			}
		} else {
			emit(Error(QString::fromLocal8Bit("Could not read block: ") + QString::number(i, 10)));
			emit(Finished(-1));
			return;
		}
		emit(Progress((unsigned int)i, (unsigned int)block_count));
	}
}

void VHDExtractor::ExtractToFile(VHDFile &vhd, QFile &file)
{
	DWORD block_size = vhd.BlockSize();
	DWORD block_count = vhd.BlocksCount();
	char *buff = new char[block_size];
	if (!buff) {
		emit(Error(QString::fromLocal8Bit("Could not allocate buffer: ") + QString::number(block_size, 10)));
		emit(Finished(-1));
		return;
	}
	memset(buff, 0x00, block_size);
	for (DWORD i = 0; i < block_count; i++) {
		if (vhd.ReadBlock(i, buff)) {
			if (file.write(buff, block_size) < block_size) {
				emit(Error(QString::fromLocal8Bit("Could not write block: ") + QString::number(i, 10)));
				emit(Finished(-1));
				return;
			}
		} else {
			emit(Error(QString::fromLocal8Bit("Could not read block: ") + QString::number(i, 10)));
			emit(Finished(-1));
			return;
		}
		emit(i, block_count);
	}
}
