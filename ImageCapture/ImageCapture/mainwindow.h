#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

private:
	Ui::MainWindow ui;
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

	bool Initialize(void);
};

#endif // MAINWINDOW_H
