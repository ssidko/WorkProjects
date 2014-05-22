#include "global.h"
#include <QFile>
#include <QByteArray>
#include <QCryptographicHash>

QString ComputeMd5FromFile(QString file_path)
{
	QString md5 = "";
	QFile file(file_path);
	if (file.open(QIODevice::ReadOnly)) {
		QByteArray buffer(file.size(), 0x00);
		int size = file.size();
		if (buffer.size()) {
			md5 = QCryptographicHash::hash(buffer, QCryptographicHash::Md5).toHex();
		}
	}
	return md5;
}

