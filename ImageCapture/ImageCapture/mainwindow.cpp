#include "mainwindow.h"
#include <QVideoWidget>
#include <QCamera>

#define CAMERA_NAME					"iLook 300"
//#define CAMERA_NAME				"ASUS USB2.0 Webcam"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	//Initialize();
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
		if (description == CAMERA_NAME) {
			camera = new QCamera(device_name);
			if (camera) {
				camera->setCaptureMode(QCamera::CaptureStillImage);
				camera->setViewfinder((QVideoWidget *)ui.ScreenWidget);
				camera->start();
			}
		}
	}

	return true;
}
