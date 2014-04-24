#include "NewTaskDialog.h"
#include <QFileDialog>

NewTaskDialog::NewTaskDialog(QWidget *parent)
	: QDialog(parent), task(NULL)
{
	ui.setupUi(this);
	UpdateWindowTitle();
	UpdateTemplatesComboBox();
	connect(ui.TaskNameEdit, SIGNAL(textChanged(const QString &)), SLOT(UpdateWindowTitle()));
	connect(ui.DirectoryPushButton, SIGNAL(clicked(bool)), SLOT(SelectTaskDirectory()));
	connect(ui.CreateTaskPushButton, SIGNAL(clicked(bool)), SLOT(CreateNewTask()));
}

NewTaskDialog::~NewTaskDialog()
{

}

void NewTaskDialog::UpdateWindowTitle()
{
	setWindowTitle(QString::fromLocal8Bit("Новая задача: ") + ui.TaskNameEdit->text());
}

void NewTaskDialog::UpdateTemplatesComboBox( void )
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

void NewTaskDialog::CreateNewTask(void)
{
	QString name = ui.TaskNameEdit->text();
	QString directory = ui.DirectoryLineEdit->text();
	task = new Task();
	if (task) {
		if (task->Create(name, directory)) {
			task->SetDescription(ui.TaskDescriptionEdit->text());
		}
	}
}

void NewTaskDialog::SelectTaskDirectory(void)
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
