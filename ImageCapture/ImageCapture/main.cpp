#include "global.h"
#include "mainwindow.h"
#include <QtWidgets/QApplication>


//=-=-=-=-==-=-=-=-===-===-=-=-=-=-=-===---=-=-==-=-
// После отладки удалить!!!
//
#include "WinDevicesManager.h"
#include "WriterWindow.h"
#include "Template.h"
#include "ControlUnit.h"
#include "Task.h"
#include "DirectShow.h"
#include <QSettings>
#include "windows.h"
#include "TCHAR.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;

	QCoreApplication::setOrganizationName(QString::fromLocal8Bit(ORGANIZATION_NAME));
	QCoreApplication::setApplicationName(QString::fromLocal8Bit(APPLICATION_NAME));
	a.setWindowIcon(QIcon(":/MainWindow/images/app.png"));
	
	w.show();
	w.activateWindow();

	return a.exec();
}