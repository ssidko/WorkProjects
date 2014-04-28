#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "Task.h"
#include <QCamera>
#include <QCameraImageCapture>
#include <QVideoWidget>

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	Ui::MainWindow ui;
	Task *task;
	QCamera *camera;
	QCameraImageCapture *image_capture;
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	bool Initialize(void);
public slots:
	void SetTask(Task *new_task);
	void TakeScreenshot(void);
};

#endif // MAINWINDOW_H
