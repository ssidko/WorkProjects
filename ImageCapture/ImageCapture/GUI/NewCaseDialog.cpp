#include "NewCaseDialog.h"
#include <QFileDialog>

NewCaseDialog::NewCaseDialog(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	UpdateWindowTitle();
	UpdateTemplatesComboBox();
	connect(ui.case_name_edit, SIGNAL(textChanged(const QString &)), SLOT(UpdateWindowTitle()));
	connect(ui.DirectoryPushButton, SIGNAL(clicked(bool)), SLOT(SelectTaskDirectory()));
}

NewCaseDialog::~NewCaseDialog()
{

}

void NewCaseDialog::UpdateWindowTitle()
{
	this->setWindowTitle(QString::fromLocal8Bit("Новая задача: ") + ui.case_name_edit->text());
}

void NewCaseDialog::UpdateTemplatesComboBox( void )
{
	QStringList templates = Template::AllTemplates();
	QString name = "";
	QString description = "";
	for (int i = 0; i < templates.size(); i++) {
		name = Template::TemplateName(templates[i]);
		description = Template::TemplateDescription(templates[i]);
		if (name != "") {
			QVariant template_path(templates[i]);
			QString item_name = name + "; " + description;
			ui.TemplateComboBox->addItem(item_name, template_path);
			name;
		}
	}
}

void NewCaseDialog::CreateNewTask(void)
{

}

void NewCaseDialog::SelectTaskDirectory(void)
{
	QFileDialog file_dialog;
	file_dialog.setFileMode(QFileDialog::Directory);
	file_dialog.setViewMode(QFileDialog::List);
	file_dialog.setWindowTitle(QString::fromLocal8Bit("Выброр каталога"));
	file_dialog.exec();
	QString dir = file_dialog.directory().absolutePath();
	ui.DirectoryLineEdit->setText(file_dialog.directory().absolutePath());
	this->activateWindow();
}
