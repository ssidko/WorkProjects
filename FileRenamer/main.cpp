#include "mainwindow.h"
#include <QtWidgets/QApplication>
#include <QFile>
#include <QTextStream>

void Test(void);

int main(int argc, char *argv[])
{
	Test();
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}

void Test(void)
{
	QFile test_file("test.ini");
	if (test_file.open(QIODevice::ReadWrite|QIODevice::Text)) {
		QTextStream text_stream(&test_file);
		QString line = text_stream.readLine();
		while (!line.isNull()) {
			line = text_stream.readLine();
		}
		int x=0;
	}
	int x = 3;
}
