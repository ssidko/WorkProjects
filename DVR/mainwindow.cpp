#include "mainwindow.h"
#include "utility.h"

#include "dhfs.h"
#include "G2fdbRecovery.h"
#include "Hikvision.h"
#include "WFS.h"

#include <QDir>
#include <QFileDialog>

#define HIKVISION_ID_STRING				"Hikvision"
#define WFS_04_ID_STRING				"WFS 0.4"
#define DHFS_ID_STRING					"DHFS"
#define G2FDB_ID_STRING					"G2FDb"

#define MAIN_WINDOW_TITLE				"DVR"

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
	this->setWindowTitle(MAIN_WINDOW_TITLE);
	InitializeDvrTypeComboBox();
	UpdateDrivesComboBox();

	QObject::connect(ui.Start_pushButton, &QPushButton::clicked, this, &MainWindow::OnStart);
	QObject::connect(ui.SelectOutFolder_pushButton, &QPushButton::clicked, this, &MainWindow::OnSelectOutDirectory);
	QObject::connect(ui.SelectInputFile_pushButton, &QPushButton::clicked, this, &MainWindow::OnSelectInputFile);
}

void MainWindow::InitializeDvrTypeComboBox(void)
{
	ui.DvrType_comboBox->addItem(DHFS_ID_STRING);
	ui.DvrType_comboBox->addItem(WFS_04_ID_STRING);
	ui.DvrType_comboBox->addItem(HIKVISION_ID_STRING);
	ui.DvrType_comboBox->addItem(G2FDB_ID_STRING);
}

void MainWindow::UpdateDrivesComboBox(void)
{
	std::list<std::string> drives;
	EnumeratePhysicalDrives([&drives](const std::string &drive_name) {drives.push_back(drive_name);});
	for (auto drive_name : drives) {
		ui.Drives_comboBox->addItem(drive_name.c_str());	
	}
}

void MainWindow::OnStart(void)
{
	QString dvr_type;
	QString io_name;
	QString out_directory;

	dvr_type = ui.DvrType_comboBox->currentText();

	if (ui.InputFile_lineEdit->isEnabled()) {
		io_name = ui.InputFile_lineEdit->text();
		if (io_name.isEmpty()) {
			io_name = OnSelectInputFile();
		}
	} else if (ui.Drives_comboBox->isEnabled()) {
		io_name = ui.Drives_comboBox->currentText();
	}

	out_directory = ui.OutFolder_lineEdit->text();
	if (out_directory.isEmpty()) {
		out_directory = OnSelectOutDirectory();	
	}

	if (!dvr_type.isEmpty() && !io_name.isEmpty() && !out_directory.isEmpty()) {

		QString title = MAIN_WINDOW_TITLE;
		title +=" -- In progress";

		this->setWindowTitle(title);

		if (dvr_type == DHFS_ID_STRING) {
			DHFS::StartRecovering(io_name.toStdString(), out_directory.toStdString());
		} else if (dvr_type == G2FDB_ID_STRING) {
			G2FDB::StartRecovery(io_name.toStdString(), out_directory.toStdString());
		} else if (dvr_type == HIKVISION_ID_STRING) {		
			HIKV::StartRecovery(io_name.toStdString(), out_directory.toStdString());
		} else if (dvr_type == WFS_04_ID_STRING) {
			WFS::StartRecovery(io_name.toStdString(), out_directory.toStdString());
		}

		title = MAIN_WINDOW_TITLE;
		title += " -- Finished";

		this->setWindowTitle(title);
	}

	return;
}

QString MainWindow::OnSelectOutDirectory(void)
{
	QString directory = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this, tr("Save to")).trimmed());
	if (!directory.isEmpty()) {
		if (directory[directory.length() - 1] != QDir::separator()) {
			directory += QDir::separator();
		}
		ui.OutFolder_lineEdit->setText(directory);
	}
	return directory;
}

QString MainWindow::OnSelectInputFile(void)
{
	QString file = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, tr("Open file")).trimmed());
	if (!file.isEmpty()) {
		ui.InputFile_lineEdit->setText(file);
	}
	return file;
}
