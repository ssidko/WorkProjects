#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	Ui::MainWindowClass ui;

	QString SizeToString(uint64_t size);

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	void Initialize(void);
	void InitWindowTitle(void);
	void InitializeDvrTypeComboBox(void);
	void UpdateDrivesComboBox(void);


public slots:
	void OnStart(void);
	QString OnSelectOutDirectory(void);
	QString OnSelectInputFile(void);

};

#endif // MAINWINDOW_H
