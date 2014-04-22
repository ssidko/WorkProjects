#include "mainwindow.h"
#include <QVideoWidget>
#include <QCamera>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	Initialize();
}

MainWindow::~MainWindow()
{

}

bool MainWindow::Initialize()
{
	QVideoWidget *wdgt = NULL;
	QCamera *camera = NULL;
	QString description;
	QString tst;
	foreach(const QByteArray &device_name, QCamera::availableDevices()) {
		description = QCamera::deviceDescription(device_name);
		if (description == "iLook 300") {
			camera = new QCamera(device_name);
			if (camera) {
				//camera->setCaptureMode(QCamera::CaptureStillImage);
				//camera->setViewfinder((QVideoWidget *)ui.ScreenWidget);
				//camera->start();
			}
				tst = "";

		}
	}

	return true;
}
