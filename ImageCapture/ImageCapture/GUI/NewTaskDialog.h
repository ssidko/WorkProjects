#ifndef NEWCASEDIALOG_H
#define NEWCASEDIALOG_H

#include <QDialog>
#include "ui_NewTaskDialog.h"
#include "Template.h"
#include "Task.h"

class NewTaskDialog : public QDialog
{
	Q_OBJECT
private:
	Ui::NewTaskDialog ui;
	Task *task;
public:
	NewTaskDialog(QWidget *parent = 0);
	~NewTaskDialog();
private slots:
	void UpdateWindowTitle();
	void UpdateTemplatesComboBox(void);
	void CreateNewTask(void);
	void SelectTaskDirectory(void);
};

#endif // NEWCASEDIALOG_H
