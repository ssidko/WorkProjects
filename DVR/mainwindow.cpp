#include "Validators.h"

#include "mainwindow.h"
#include "utility.h"
#include "version.h"

//#include "dhfs.h"
#include "G2fdbRecovery.h"
#include "DhfsRecovery.h"
#include "Hikvision.h"
#include "WFS.h"
#include "Orbita.h"

// DVR Property forms
#include "G2FDbPropertyForm.h"

#include <QDir>
#include <QFileDialog>
#include <QDateTimeEdit>
#include <QMessageBox>

#define HIKVISION_ID_STRING						"Hikvision"
#define WFS_04_ID_STRING						"WFS 0.4"
#define DHFS_ID_STRING							"DHFS"
#define G2FDB_ID_STRING							"G2FDb"
#define G2FDB_METADATA_ID_STRING				"G2FDb metadata"
#define DCH264_ID_STRING						"dcH264"

#define APP_NAME						"DVR"

MainWindow::MainWindow(QWidget *parent) : 
	QMainWindow(parent),
	property_form(nullptr),
	current_dvr_type("")
{
	ui.setupUi(this);
	Initialize();
}

MainWindow::~MainWindow()
{
}

QString MainWindow::SizeToString(uint64_t size)
{
	float num = size;
	QStringList list;
	list << "KB" << "MB" << "GB" << "TB";

	QStringListIterator i(list);
	QString unit("bytes");

	while (num >= 1024.0 && i.hasNext()) {
		unit = i.next();
		num /= 1024.0;
	}
	return QString().setNum(num, 'f', 1) + " " + unit;
}

void MainWindow::Initialize(void)
{
	InitWindowTitle();
	UpdateDrivesComboBox();

	ui.Offset_lineEdit->setValidator(new QUInt64Validator(this));

	QObject::connect(ui.Start_pushButton, &QPushButton::clicked, this, &MainWindow::OnStart);
	QObject::connect(ui.SelectOutFolder_pushButton, &QPushButton::clicked, this, &MainWindow::OnSelectOutDirectory);
	QObject::connect(ui.SelectInputFile_pushButton, &QPushButton::clicked, this, &MainWindow::OnSelectInputFile);

	QObject::connect(ui.DvrType_comboBox, &QComboBox::currentTextChanged, this, &MainWindow::SetDvrType);
	InitializeDvrTypeComboBox();
}

void MainWindow::InitWindowTitle(void)
{
	QString title = "";
	title.sprintf("%s  [ver: %d.%d.%d]", APP_NAME, AppVersion::kRelease, AppVersion::kMajor, AppVersion::kMinor);
	this->setWindowTitle(title);
}

void MainWindow::InitializeDvrTypeComboBox(void)
{
	ui.DvrType_comboBox->addItem(DHFS_ID_STRING);
	ui.DvrType_comboBox->addItem(WFS_04_ID_STRING);
	ui.DvrType_comboBox->addItem(HIKVISION_ID_STRING);
	ui.DvrType_comboBox->addItem(G2FDB_ID_STRING);
	ui.DvrType_comboBox->addItem(G2FDB_METADATA_ID_STRING);
	ui.DvrType_comboBox->addItem(DCH264_ID_STRING);
}

void MainWindow::UpdateDrivesComboBox(void)
{
	std::list<std::string> drives;
	EnumeratePhysicalDrives([&drives](const std::string &drive_name) {drives.push_back(drive_name);});
	for (auto drive_name : drives) {
		LONGLONG drive_size = GetPhysicalDriveSize(drive_name);
		QString item_text = drive_name.c_str();
		item_text += "; ";
		item_text += SizeToString(drive_size);
		ui.Drives_comboBox->addItem(item_text);
	}
}

void MainWindow::SetDvrType(const QString &dvr_type)
{
	if (current_dvr_type != dvr_type) {
		current_dvr_type = dvr_type;

		if (property_form) {
			delete property_form;
			property_form = nullptr;
			ui.centralWidget->adjustSize();
			this->adjustSize();
		}

		if (dvr_type == G2FDB_ID_STRING) {

			property_form = new G2FDbPropertyForm(this);
			property_form->show();
			property_form->adjustSize();

			QSize size = this->size();
			QSize prop_size = property_form->size();
			this->resize(size.width(), size.height() + property_form->size().height());
			//this->setMinimumSize(this->size());

			ui.main_verticalLayout->insertWidget(1, property_form);

		}	
	}
}

bool MainWindow::InitRecoveryTask(dvr::RecoveryTask & task)
{
	bool block_device = false;
	uint64_t io_size = 0;

	task.io_name = "";
	if (ui.InputFile_lineEdit->isEnabled()) {
		block_device = false;
		task.io_name = ui.InputFile_lineEdit->text().toStdString();
		if (task.io_name.empty()) {
			throw std::runtime_error("Input file is not specified");
		}
		QFileInfo check_file(task.io_name.c_str());
		if (!check_file.exists() ) {
			throw std::runtime_error("Input file is not exist");
		}
		if (!check_file.isFile()) {
			throw std::runtime_error("Input file is not a file");
		}
		io_size = check_file.size();
		if (!io_size) {
			throw std::runtime_error("Input file zero sized");
		}
	} else if (ui.Drives_comboBox->isEnabled()) {
		block_device = true;
		if (ui.Drives_comboBox->count()) {
			task.io_name = ui.Drives_comboBox->currentText().split(QChar(';')).at(0).toStdString();		
		} else {
			throw std::runtime_error("No available physical drives");
		}
		if (task.io_name.find("\\\\.\\PhysicalDrive") == std::string::npos) {
			throw std::runtime_error("Wrong physical drive name");
		}
		if (auto size = GetPhysicalDriveSize2(task.io_name)) {
			io_size = size.value();
		} else {
			throw std::runtime_error("Failed: GetPhysicalDriveSize()");
		}
	}

	task.io_offset = static_cast<uint64_t>(ui.Offset_lineEdit->text().toULongLong());

	if (block_device) {
		if ((task.io_offset & (512 - 1)) != 0) {
			throw std::runtime_error("Offset must be multiple of sector size");
		}
	}
	if (task.io_offset >= io_size) {
		throw std::runtime_error("Offset must be less then IO size");
	}

	task.io_size = io_size - task.io_offset;
	task.output_dir = ui.OutFolder_lineEdit->text().toStdString();

	if (task.output_dir.empty()) {
		throw std::runtime_error("Destination directory is not specified");
	}
	QFileInfo check_dir(task.output_dir.c_str());
	if (!check_dir.exists()) {
		throw std::runtime_error("Destination directory is not exist");
	}
	if (!check_dir.isDir()) {
		throw std::runtime_error("Destionation directory is not a directory");
	}

	QDateTime date_time;
	if (ui.start_dateTimeEdit->isEnabled()) {
		date_time = ui.start_dateTimeEdit->dateTime();
		task.start_time.year = date_time.date().year();
		task.start_time.month = date_time.date().month();
		task.start_time.day = date_time.date().day();
		task.start_time.hours = date_time.time().hour();
		task.start_time.minutes = date_time.time().minute();
		task.start_time.seconds = date_time.time().second();
	}
	if (ui.end_dateTimeEdit->isEnabled()) {
		date_time = ui.end_dateTimeEdit->dateTime();
		task.end_time.year = date_time.date().year();
		task.end_time.month = date_time.date().month();
		task.end_time.day = date_time.date().day();
		task.end_time.hours = date_time.time().hour();
		task.end_time.minutes = date_time.time().minute();
		task.end_time.seconds = date_time.time().second();
	}

	return true;
}

void MainWindow::OnStart(void)
{
	QString dvr_type;
	QString io_name;
	QString out_directory;

	QDateTime start_date = ui.start_dateTimeEdit->dateTime();
	QDateTime end_date = ui.end_dateTimeEdit->dateTime();

	dvr::Timestamp start_tstmp(start_date.date().year(), start_date.date().month(),
		start_date.date().day(), start_date.time().hour(), start_date.time().second());
	dvr::Timestamp end_tstmp(end_date.date().year(), end_date.date().month(),
		end_date.date().day(), end_date.time().hour(), end_date.time().second());

	dvr_type = ui.DvrType_comboBox->currentText();

	uint64_t offset = static_cast<uint64_t>(ui.Offset_lineEdit->text().toULongLong());

	if (ui.InputFile_lineEdit->isEnabled()) {
		io_name = ui.InputFile_lineEdit->text();
		if (io_name.isEmpty()) {
			io_name = OnSelectInputFile();
		}
	} else if (ui.Drives_comboBox->isEnabled()) {
		io_name = ui.Drives_comboBox->currentText().split(QChar(';')).at(0);
	}

	out_directory = ui.OutFolder_lineEdit->text();
	if (out_directory.isEmpty()) {
		out_directory = OnSelectOutDirectory();	
	}


	dvr::RecoveryTask task;
	try {
		InitRecoveryTask(task);
	} catch (std::exception &exc) {
		QMessageBox msg_box;
		msg_box.setText(exc.what());
		msg_box.exec();
	}

	if (!dvr_type.isEmpty() && !io_name.isEmpty() && !out_directory.isEmpty()) {

		QString title = APP_NAME;
		title +=" -- In progress";

		this->setWindowTitle(title);

		if (dvr_type == DHFS_ID_STRING) {
			//DHFS::StartRecovering(io_name.toStdString(), out_directory.toStdString());
			DHFS::StartRecovery(io_name.toStdString(), offset, out_directory.toStdString());
		} else if (dvr_type == G2FDB_ID_STRING) {
			//G2FDB::StartRecovery(io_name.toStdString(), out_directory.toStdString());
			G2FDB::StartRecovery(task);
		} else if (dvr_type == G2FDB_METADATA_ID_STRING) {
			G2FDB::StartRcoveryByMetadata(task);
		} else if (dvr_type == HIKVISION_ID_STRING) {
			HIKV::StartRecovery(io_name.toStdString(), out_directory.toStdString());
		} else if (dvr_type == WFS_04_ID_STRING) {
			WFS::StartRecovery(io_name.toStdString(), out_directory.toStdString());
		} else if (dvr_type == DCH264_ID_STRING) {
			Orbita::Main(io_name.toStdString(), out_directory.toStdString());
		} 

		title = APP_NAME;
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
