#include "DiskImageExtractor.h"

DiskImageExtractor::DiskImageExtractor(DiskImageFile *disk_image_file, QString output_file)
{
	image_file = disk_image_file;
	output_file_name = output_file.trimmed();
}

DiskImageExtractor::~DiskImageExtractor(void)
{
}

void DiskImageExtractor::run()
{
	if (output_file_name.contains(QString::fromLocal8Bit("\\\\.\\PhysicalDrive"), Qt::CaseInsensitive)) {
		PhysicalDrive disk(output_file_name.toLocal8Bit().data());
		if (!disk.Open()) {
			emit(Error(QString::fromLocal8Bit("Could not open disk: \n") + output_file_name));
			emit(Finished(-1));
			return;
		} else {
			ExtractToDisk(*image_file, disk);
		}
	} else {
		QFile out_file(output_file_name);
		if (!out_file.open(QIODevice::ReadWrite)) {
			emit(Error(QString::fromLocal8Bit("Could not open out file: \n") + output_file_name));
			emit(Finished(-1));
			return;
		} else {
			ExtractToFile(*image_file, out_file);
		}
	}
	emit(Finished(0));
}

void DiskImageExtractor::ExtractToDisk(DiskImageFile &image_file, PhysicalDrive &output_disk)
{
	DWORD block_size = image_file.BlockSize();
	DWORD block_count = image_file.BlocksCount();
	char *buff = new char[block_size];
	if (!buff) {
		emit(Error(QString::fromLocal8Bit("Could not allocate buffer: ") + QString::number(block_size, 10) + QString::fromLocal8Bit(" bytes")));
		emit(Finished(-1));
		return;
	}
	memset(buff, 0x00, block_size);
	for (DWORD i = 0; i < block_count; i++) {
		if (image_file.ReadBlock(i, buff)) {
			if (output_disk.Write(buff, block_size) < block_size) {
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

void DiskImageExtractor::ExtractToFile(DiskImageFile &image_file, QFile &output_file)
{
	DWORD block_size = image_file.BlockSize();
	DWORD block_count = image_file.BlocksCount();
	char *buff = new char[block_size];
	if (!buff) {
		emit(Error(QString::fromLocal8Bit("Could not allocate buffer: ") + QString::number(block_size, 10) + QString::fromLocal8Bit(" bytes")));
		emit(Finished(-1));
		return;
	}
	memset(buff, 0x00, block_size);
	for (DWORD i = 0; i < block_count; i++) {
		if (image_file.ReadBlock(i, buff)) {
			if (output_file.write(buff, block_size) < block_size) {
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
