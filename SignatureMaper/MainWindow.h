#ifndef SIGNATUREMAPER_H
#define SIGNATUREMAPER_H

#include <QtWidgets/QMainWindow>
#include "ui_signaturemaper.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();

private:
	Ui::SignatureMaperClass ui;
};

#endif // SIGNATUREMAPER_H
