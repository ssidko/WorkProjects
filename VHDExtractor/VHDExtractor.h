#ifndef VHDEXTRACTOR_H
#define VHDEXTRACTOR_H

#include <QThread>
#include <QFile>
#include "PhysicalDrive.h"
#include "VHDFile.h"

using namespace W32Lib;

class VHDExtractor : public QThread
{
	Q_OBJECT
private:
	QString vhd_file_name;
	QString out_file_name;
	void ExtractToDisk(VHDFile &vhd, PhysicalDrive &disk);
	void ExtractToFile(VHDFile &vhd, QFile &file);
protected:
	void run();
public:
	explicit VHDExtractor(QString vhd_file, QString out_file);
	~VHDExtractor(void);
signals:
	void Finished(int exit_code);
	void Progress(unsigned int current_block, unsigned int max_block);
	void Error(const QString &error_string);
};

#endif

