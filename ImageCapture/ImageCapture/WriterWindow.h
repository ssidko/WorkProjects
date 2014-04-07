#ifndef WRITERWINDOW_H
#define WRITERWINDOW_H

#include <QMainWindow>
#include "ui_WriterWindow.h"

class WriterWindow : public QMainWindow
{
	Q_OBJECT

public:
	WriterWindow(QWidget *parent = 0);
	~WriterWindow();
private:
	Ui::WriterWindow ui;
	QString file_path;
private slots:
	void SelectFile(void);
	void UpdateComPortsList(void);
	void StartTransfer(void);
};

#endif // WRITERWINDOW_H
