#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"

class VHDExtractor : public QMainWindow
{
	Q_OBJECT

public:
	VHDExtractor(QWidget *parent = 0);
	~VHDExtractor();

private:
	Ui::VHDExtractorClass ui;
};

#endif // MAINWINDOW_H
