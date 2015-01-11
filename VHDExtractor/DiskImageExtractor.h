#ifndef VHDEXTRACTOR_H
#define VHDEXTRACTOR_H

#include <fstream>
#include <QThread>
#include <QFile>
#include <QMessageBox>
#include "PhysicalDrive.h"
#include "DiskImageFile.h"
#include "VHDFile.h"

using namespace W32Lib;

class DiskImageExtractor : public QThread
{
	Q_OBJECT
private:
	DiskImageFile *image_file;
	QString output_file_name;
	void ExtractToDisk(DiskImageFile &image_file, PhysicalDrive &output_disk);
	void ExtractToFile(DiskImageFile &image_file, QFile &output_file);
protected:
	void run();
public:
	explicit DiskImageExtractor(DiskImageFile *disk_image_file, QString output_file);
	~DiskImageExtractor(void);
	void Extract(DiskImageFile &image, QString &out_file_name);
signals:
	void Finished(int exit_code);
	void Progress(unsigned int current_block, unsigned int max_block);
	void Error(const QString &error_string);
};

#endif

