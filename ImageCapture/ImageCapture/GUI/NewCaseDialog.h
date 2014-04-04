#ifndef NEWCASEDIALOG_H
#define NEWCASEDIALOG_H

#include <QDialog>
#include "ui_NewCaseDialog.h"

class NewCaseDialog : public QDialog
{
	Q_OBJECT

public:
	NewCaseDialog(QWidget *parent = 0);
	~NewCaseDialog();

private:
	Ui::NewCaseDialog ui;

private slots:
	void UpdateWindowTitle();
	
};

#endif // NEWCASEDIALOG_H
