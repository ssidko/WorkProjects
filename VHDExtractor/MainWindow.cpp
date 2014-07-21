#include "MainWindow.h"
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>

#define FILE_PATH_LINE_EDIT_DEFAULT_STRING					"Select disk"

QStringList AvailablePhysicalDrives(void)
{
	QStringList disk_list;
	QString disk_name;
	for (int i = 0; i < 128; i++) {
		disk_name = "\\\\.\\PhysicalDrive" + QString::number(i, 10);
		QFile disk(disk_name);
		if (disk.open(QIODevice::ReadOnly)) {
			char buff[512] = {0};
			if (disk.read(buff, 512)) {
				disk_list.push_back(disk_name);
			}
		}
	}
	return disk_list;
}

MainWindow::MainWindow(QWidget *parent) 
	: QMainWindow(parent),
	progress_bar(NULL),
	progress_text(NULL)
{
	ui.setupUi(this);
	InitializeWidgets();
}

MainWindow::~MainWindow()
{

}

void MainWindow::InitializeWidgets(void)
{
	progress_text = new QLabel(this);
	ui.statusBar->addWidget(progress_text);
	progress_text->setFixedWidth(100);
	progress_text->setAlignment(Qt::AlignRight);

	progress_bar = new QProgressBar(this);
	ui.statusBar->addWidget(progress_bar);
	progress_bar->setFixedHeight(15);
	progress_bar->setFixedWidth(200);

	ui.DisksComboBox->addItem(QString::fromLocal8Bit(FILE_PATH_LINE_EDIT_DEFAULT_STRING));
	ui.DisksComboBox->addItems(AvailablePhysicalDrives());

	connect(ui.FilePathLineEdit, SIGNAL(textChanged(const QString &)), SLOT(UpdateStartButtonState(void)));
	connect(ui.DisksComboBox, SIGNAL(currentIndexChanged(const QString &)), SLOT(UpdateStartButtonState(void)));
	connect(ui.OpenFilePushButton, SIGNAL(clicked(bool)), SLOT(SelectVHDFile(void)));

	//HideProgress();
	ShowProgress();
	UpdateStartButtonState();
}

void MainWindow::ShowProgress()
{
	ui.statusBar->show();
}

void MainWindow::HideProgress()
{
	ui.statusBar->hide();
}

bool MainWindow::IsValidParameters(void)
{
	QString file_path = ui.FilePathLineEdit->text().trimmed();
	QString disk_name = ui.DisksComboBox->currentText();
	QFileInfo file_info(file_path);
	if (file_info.exists() && (disk_name != QString::fromLocal8Bit(FILE_PATH_LINE_EDIT_DEFAULT_STRING))) {
		return true;
	} else {
		return false;
	}
}

void MainWindow::UpdateStartButtonState(void)
{
	if (IsValidParameters()) {
		ui.StartPushButton->setEnabled(true);
	} else {
		ui.StartPushButton->setEnabled(false);
	}
}

void MainWindow::SelectVHDFile(void)
{
	QString vhd_file_name = QFileDialog::getOpenFileName();
	ui.FilePathLineEdit->setText(vhd_file_name);
}

void MainWindow::SrartExtraction()
{

}
