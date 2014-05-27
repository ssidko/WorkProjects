#include "global.h"
#include <QFile>
#include <QByteArray>
#include <QCryptographicHash>

QString ComputeMd5(QString file_path)
{
	QString md5 = "";
	QFile file(file_path);
	if (file.open(QIODevice::ReadOnly)) {
		QByteArray buffer(file.size(), 0x00);
		buffer = file.readAll();
		if (buffer.size()) {
			md5 = QCryptographicHash::hash(buffer, QCryptographicHash::Md5).toHex();
		} else {
			// В процессе чтения файла возникли ошибки.
		}
	} else {
		// Не удалось открыть файл.
	}
	return md5;
}

