#ifndef NEWCASEDIALOG_H
#define NEWCASEDIALOG_H

#include <QDialog>
#include "ui_NewCaseDialog.h"
#include "Template.h"
#include "Task.h"

class NewCaseDialog : public QDialog
{
	Q_OBJECT
private:
	Ui::NewCaseDialog ui;
	Task new_task;

public:
	NewCaseDialog(QWidget *parent = 0);
	~NewCaseDialog();

private slots:
	void UpdateWindowTitle();
	void UpdateTemplatesComboBox(void);
	void CreateNewTask(void);	
};

#endif // NEWCASEDIALOG_H
