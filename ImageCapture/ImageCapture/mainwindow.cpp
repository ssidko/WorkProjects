#include "mainwindow.h"

#define CAMERA_NAME					"USB 2861 Video"
//#define CAMERA_NAME				"iLook 300"
//#define CAMERA_NAME				"ASUS USB2.0 Webcam"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), task(NULL), camera(NULL), image_capture(NULL)
{
	ui.setupUi(this);
	Initialize();
	connect(ui.ScreenshotButton, SIGNAL(clicked(bool)), SLOT(TakeScreenshot(void)));
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
				camera->setViewfinder((QVideoWidget *)ui.ScreenWidget);
				camera->setCaptureMode(QCamera::CaptureStillImage);
				image_capture = new QCameraImageCapture(camera);
				image_capture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
				
				QImageEncoderSettings image_settings;
				image_settings.setCodec("image/jpeg");
				image_settings.setResolution(640, 480);
				image_capture->setEncodingSettings(image_settings);

				camera->start();
			}
		}
	}

	return true;
}

void MainWindow::SetTask(Task *new_task)
{
	//
	// SaveCurrentTask();
	//

	if (new_task) {
		task = new_task;
		ui.TaskTreeWidget->setColumnCount(1);
		ui.TaskTreeWidget->setHeaderLabel(QString::fromLocal8Bit("Задача"));

		QTreeWidgetItem *task_item = new QTreeWidgetItem(ui.TaskTreeWidget);
		task_item->setText(0, task->Name());
		task_item->setExpanded(true);

		QTreeWidgetItem *task_description = new QTreeWidgetItem(task_item);
		task_description->setText(0, QString::fromLocal8Bit("Описание: ") + task->Description());

		foreach(const Template &t, *task->Templates()) {
			QTreeWidgetItem *template_item = new QTreeWidgetItem(task_item);
			template_item->setText(0, t.Name() + ", " + t.Description());
			template_item->setExpanded(true);
			foreach (const SECTION &section, *t.Sections()) {
				QTreeWidgetItem *section_item = new QTreeWidgetItem(template_item);
				section_item->setText(0, section.name);
			}
		}
	}
}

void MainWindow::TakeScreenshot(void)
{
	static unsigned int counter = 0;
	if (task) {
		QString picture_name = task->ResDirectory() + "/" + QString::number(counter, 10) + ".jpg";
		camera->searchAndLock();
		image_capture->capture(picture_name);
		camera->unlock();
		counter++;
	}
}
