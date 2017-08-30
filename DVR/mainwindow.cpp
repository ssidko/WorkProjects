#include "mainwindow.h"
#include "utility.h"
#include "version.h"

//#include "dhfs.h"
#include "G2fdbRecovery.h"
#include "DhfsRecovery.h"
#include "Hikvision.h"
#include "WFS.h"
#include "Orbita.h"

#include <QDir>
#include <QFileDialog>
#include <QDateTimeEdit>

#define HIKVISION_ID_STRING				"Hikvision"
#define WFS_04_ID_STRING				"WFS 0.4"
#define DHFS_ID_STRING					"DHFS"
#define G2FDB_ID_STRING					"G2FDb"
#define DCH264_ID_STRING				"dcH264"

#define APP_NAME						"DVR"

class QUInt64Validator : public QValidator
{
private:
	int64_t value;
public:
	QUInt64Validator(QObject * parent = 0) : QValidator(parent) {}
	~QUInt64Validator() {}
	void fixup(QString &input) const override {}
	State validate(QString &input, int &pos) const override
	{
		bool ok = false;
		uint64_t value = static_cast<uint64_t>(input.toULongLong(&ok, 10));
		if (ok) {
			return QValidator::Acceptable;
		} else {
			return QValidator::Invalid;
		}
	}
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
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
	InitializeDvrTypeComboBox();
	UpdateDrivesComboBox();

	ui.Offset_lineEdit->setValidator(new QUInt64Validator(this));

	QObject::connect(ui.Start_pushButton, &QPushButton::clicked, this, &MainWindow::OnStart);
	QObject::connect(ui.SelectOutFolder_pushButton, &QPushButton::clicked, this, &MainWindow::OnSelectOutDirectory);
	QObject::connect(ui.SelectInputFile_pushButton, &QPushButton::clicked, this, &MainWindow::OnSelectInputFile);
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

	if (!dvr_type.isEmpty() && !io_name.isEmpty() && !out_directory.isEmpty()) {

		QString title = APP_NAME;
		title +=" -- In progress";

		this->setWindowTitle(title);

		if (dvr_type == DHFS_ID_STRING) {
			//DHFS::StartRecovering(io_name.toStdString(), out_directory.toStdString());
			DHFS::StartRecovery(io_name.toStdString(), offset, out_directory.toStdString());
		} else if (dvr_type == G2FDB_ID_STRING) {
			G2FDB::StartRecovery(io_name.toStdString(), out_directory.toStdString());
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
