#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"
#include "Task.h"
#include <QCamera>
#include <QCameraImageCapture>
#include <QVideoWidget>
#include "NewTaskDialog.h"
#include "Template.h"
#include "ControlUnit.h"

class CmdButton : public QPushButton
{
	Q_OBJECT
private:
	QString cmd;
public:
	CmdButton(const QString &command, QWidget *parent = 0) : cmd(command), QPushButton(parent)
	{
		connect(this, SIGNAL(clicked(bool)), SLOT(ButtonClicked()));
	}
public slots:
	void ButtonClicked(void)
	{
		emit SendCommand(cmd);
	}
signals:
	void SendCommand(QString command);

};

class MainWindow : public QMainWindow
{
	Q_OBJECT
private:
	Ui::MainWindow ui;
	Task *task;
	QCamera *camera;
	QCameraImageCapture *image_capture;
	ControlUnit control_unit;
	QList<CmdButton *> buttons;
	QTreeWidgetItem *current_item;
	QString MakeTemplateItemName(const Template &t);
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	bool Initialize(void);
	bool CreateButtons(const Template &t);
	void DestroyButtons(void);
	Template *CurrentTemplate(void);
public slots:
	bool CreateNewTask(void);
	bool SaveTask(void);
	void SetTask(Task *new_task);
	void TakeScreenshot(void);
	bool SendCommand(QString command);
	void CheckSelection(QTreeWidgetItem *current, QTreeWidgetItem *previous);
	void ChangeSection(void);
	void AddScreenshot(int id, const QString &file_name);
};

#endif // MAINWINDOW_H
