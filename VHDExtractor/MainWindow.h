#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <QProgressBar>
#include <QLabel>

QStringList AvailablePhysicalDrives(void);

class MainWindow : public QMainWindow
{
	Q_OBJECT
private:
	Ui::VHDExtractorClass ui;
	QProgressBar *progress_bar;
	QLabel *progress_text;
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	void InitializeWidgets(void);
	void ShowProgress(void);
	void HideProgress(void);
	bool IsValidParameters(void);
public slots:
	void UpdateStartButtonState(void);
	void SelectVHDFile(void);
	void SrartExtraction();
};

#endif // MAINWINDOW_H
