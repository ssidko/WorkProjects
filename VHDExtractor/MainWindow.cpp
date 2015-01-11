#include "MainWindow.h"
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include "VDIFile.h"
#include "VHDFile.h"
#include "DiskImageExtractor.h"

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

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
	progress_bar(NULL),
	progress_text(NULL),
	image_file(NULL),
	extractor(NULL)
{
	ui.setupUi(this);
	InitializeWidgets();
	//setFixedSize(geometry().width(), geometry().height());
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

	ui.DisksList_ComboBox->addItem(QString::fromLocal8Bit(FILE_PATH_LINE_EDIT_DEFAULT_STRING));
	ui.DisksList_ComboBox->addItems(AvailablePhysicalDrives());

	connect(ui.DiskImageFilePath_LineEdit, SIGNAL(textChanged(const QString &)), SLOT(UpdateStartButtonState(void)));
	connect(ui.DisksList_ComboBox, SIGNAL(currentIndexChanged(const QString &)), SLOT(UpdateStartButtonState(void)));
	connect(ui.OpenDiskImageFile_PushButton, SIGNAL(clicked(bool)), SLOT(SelectDiskImageFile(void)));
	connect(ui.StartPushButton, SIGNAL(clicked(bool)), SLOT(SrartExtraction(void)));

	EnableStatusBar(false);
	UpdateStartButtonState();
}

void MainWindow::EnableStatusBar(bool enable)
{
	if (enable) {
		ui.statusBar->show();
	} else {
		ui.statusBar->hide();
	}
}

bool MainWindow::IsValidParameters(void)
{
	QString file_path = ui.DiskImageFilePath_LineEdit->text().trimmed();
	QString disk_name = ui.DisksList_ComboBox->currentText();
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

void MainWindow::OpenDiskImageFile(void)
{
	QString image_file_name = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), "(*.vhd *.vdi)");


}

void MainWindow::SelectDiskImageFile(void)
{
	QString image_file_name;
	ui.DiskImageFilePath_LineEdit->setText(image_file_name);
}

void MainWindow::SrartExtraction()
{
	QString output_file_name = ui.DisksList_ComboBox->currentText();
	if (extractor) {
		delete extractor;
	}
	extractor = new DiskImageExtractor(image_file, output_file_name);
	if (extractor) {
		connect(extractor, SIGNAL(Finished(int)), SLOT(ExtractionFinished(int)));
		connect(extractor, SIGNAL(Progress(unsigned int,unsigned int)), SLOT(UpdateProgress(unsigned int,unsigned int)));
		connect(extractor, SIGNAL(Error(QString)), SLOT(HandleExtractionError(QString)));
		extractor->start();
		EnableUserInput(false);
		EnableStatusBar(true);
	}
}

void MainWindow::EnableUserInput(bool enable)
{
	ui.DiskImageFilePath_LineEdit->setEnabled(enable);
	ui.OpenDiskImageFile_PushButton->setEnabled(enable);
	ui.DisksList_ComboBox->setEnabled(enable);
	ui.StartPushButton->setEnabled(enable);
}

void MainWindow::ExtractionFinished(int end_code)
{
	if (end_code >= 0) {
		QMessageBox::information(NULL, "Congratulations!!!", "Extraction completed");
	}
	EnableUserInput(true);
	EnableStatusBar(false);
}

void MainWindow::UpdateProgress(unsigned int current_block, unsigned int max_block)
{
	if (progress_bar && progress_text) {
		progress_text->setText(QString::number(current_block, 10) + "/" + QString::number(max_block, 10));
		progress_bar->setMaximum(max_block-1);
		progress_bar->setValue(current_block);
	}
}

void MainWindow::HandleExtractionError(QString error_message)
{
	QMessageBox::warning(NULL, "Error", error_message);
}
