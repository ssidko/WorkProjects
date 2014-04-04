#include "NewCaseDialog.h"

NewCaseDialog::NewCaseDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	UpdateWindowTitle();

	connect(ui.case_name_edit, SIGNAL(textChanged(const QString &)), SLOT(UpdateWindowTitle()));

	int x = 0;
}

NewCaseDialog::~NewCaseDialog()
{

}

void NewCaseDialog::UpdateWindowTitle()
{
	this->setWindowTitle(QString::fromLocal8Bit("Новая задача: ") + ui.case_name_edit->text());
}
