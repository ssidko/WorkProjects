#include "mainwindow.h"

#define HIKVISION_ID_STRING			"Hikvision"
#define WFS_04_ID_STRING				"WFS 0.4"
#define DHFS_ID_STRING				"DHFS"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	ui.setupUi(this);
	Initialize();
}

MainWindow::~MainWindow()
{

}

void MainWindow::Initialize(void)
{
	this->setWindowTitle("DVR");
	InitializeDvrTypeComboBox();
}

void MainWindow::InitializeDvrTypeComboBox(void)
{
	ui.DvrType_comboBox->addItem(DHFS_ID_STRING);
	ui.DvrType_comboBox->addItem(WFS_04_ID_STRING);
	ui.DvrType_comboBox->addItem(HIKVISION_ID_STRING);
}
