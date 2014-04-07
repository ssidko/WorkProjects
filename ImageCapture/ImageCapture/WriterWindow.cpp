#include "WriterWindow.h"
#include <QFileDialog>
#include <QFile>
#include <Windows.h>

WriterWindow::WriterWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	file_path = "";
	connect(ui.SelectFileButton, SIGNAL(clicked(bool)), SLOT(SelectFile(void)));
	connect(ui.StartButton, SIGNAL(clicked(bool)), SLOT(StartTransfer()));
	UpdateComPortsList();
}

WriterWindow::~WriterWindow()
{

}

void WriterWindow::SelectFile(void)
{
	file_path = QFileDialog::getOpenFileName();
	ui.lineEdit->setText(file_path);
}

void WriterWindow::UpdateComPortsList(void)
{
	ui.comboBox->clear();
	HANDLE handle = NULL;
	for (int x = 0; x < 50; x++) {
		QString com_name = "\\\\.\\COM" + QString::number(x, 10);
		handle = ::CreateFileA(com_name.toLocal8Bit().data(), GENERIC_READ|GENERIC_WRITE, 0/*exclusive access*/, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (handle != INVALID_HANDLE_VALUE) {
			::CloseHandle(handle);
			ui.comboBox->addItem(com_name);
		}

	}
}

void WriterWindow::StartTransfer(void)
{
	if (file_path.isEmpty()) return;
	QFile src_file(file_path);
	if (!src_file.open(QIODevice::ReadOnly)) return;
	qint64 size = src_file.size();
	qint64 readed = 0;

	QString com_name = ui.comboBox->currentText();
	if (com_name.isEmpty()) return;

	HANDLE handle = NULL;
	handle = ::CreateFileA(com_name.toLocal8Bit().data(), GENERIC_READ|GENERIC_WRITE, 0/*exclusive access*/, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE) return;

	DCB com_param;
	memset(&com_param, 0x00, sizeof(com_param));
	if (!GetCommState(handle, &com_param)) return;
	com_param.BaudRate = CBR_115200;
	com_param.ByteSize = 8;
	com_param.StopBits = ONESTOPBIT;
	com_param.Parity = NOPARITY;
	if (!SetCommState(handle, &com_param)) return;


	DWORD rw = 0;
	const int buff_size = 32;
	char buff[32];
	int progress = 0;
	while (rw = (DWORD)src_file.read(buff, buff_size)) {
		if (!::WriteFile(handle, buff, buff_size, &rw, NULL)) {
			ui.progressBar->setValue(0);
			return;
		}
		readed += rw;
		progress = (int)(((double)100/(double)size)*(double)readed);
		ui.progressBar->setValue(progress);
	}
	ui.progressBar->setValue(ui.progressBar->maximum());
	::CloseHandle(handle);
}
