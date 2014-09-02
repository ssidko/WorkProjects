#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include "ui_MainWindow.h"
#include <QProgressBar>
#include <QLabel>
#include "DiskImageFile.h"
#include "DiskImageExtractor.h"

QStringList AvailablePhysicalDrives(void);

class MainWindow : public QMainWindow
{
	Q_OBJECT
private:
	Ui::VHDExtractorClass ui;
	QProgressBar *progress_bar;
	QLabel *progress_text;
	DiskImageFile *image_file;
	DiskImageExtractor *extractor;
	void EnableUserInput(bool enable);
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
	void InitializeWidgets(void);
	void EnableStatusBar(bool enable);
	void ShowProgress(void);
	void HideProgress(void);
	bool IsValidParameters(void);
public slots:
	void UpdateStartButtonState(void);
	void ChooseDiskImageFile(void);
	void SrartExtraction();
	void ExtractionFinished(int end_code);
	void UpdateProgress(unsigned int current_block, unsigned int max_block);
	void HandleExtractionError(QString error_message);
};

#endif // MAINWINDOW_H
