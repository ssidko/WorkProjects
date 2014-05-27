#include "NewTaskDialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QCoreApplication>

#define TEMPLATE_COMBOBOX_DEFAULT_STRING			"Выберите игровой автомат"
#define DIRECTORY_LINE_EDIT_DEFAULT_STRING			"Выберите каталог"

NewTaskDialog::NewTaskDialog(QWidget *parent)
	: QDialog(parent), task(NULL)
{
	ui.setupUi(this);
	UpdateWindowTitle();
	ui.TemplateComboBox->addItem(QString::fromLocal8Bit(TEMPLATE_COMBOBOX_DEFAULT_STRING));
	UpdateTemplatesComboBox();
	ui.DirectoryLineEdit->setText(QSettings().value(APP_SETTINGS_TASK_DIRECTORY, QString::fromLocal8Bit(DIRECTORY_LINE_EDIT_DEFAULT_STRING)).toString());

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
	QString template_name = ui.TemplateComboBox->currentText();
	QString template_path = "";
	QString warning = "";

	if (template_name == QString::fromLocal8Bit(TEMPLATE_COMBOBOX_DEFAULT_STRING)) {
		warning += QString::fromLocal8Bit("Необходимо выбрать игровой автомат\n");
	} 
	if (directory == QString::fromLocal8Bit(DIRECTORY_LINE_EDIT_DEFAULT_STRING)) {
		warning += QString::fromLocal8Bit("Необходимо выбрать каталог\n");
	}

	if (warning.isEmpty()) {
		task = new Task();
		if (task) {
			if (task->Create(name, directory)) {
				task->SetDescription(ui.TaskDescriptionEdit->text());
				template_path = ui.TemplateComboBox->itemData(ui.TemplateComboBox->currentIndex()).toString();
				task->AddTemplate(template_path);
				if (task->Save()) {
					QSettings().setValue(APP_SETTINGS_TASK_DIRECTORY, directory);
					accept();
				}
			}
		}
	} else {
		QMessageBox msgBox;
		msgBox.setText(warning);
		msgBox.exec();
	}
}

void NewTaskDialog::SelectTaskDirectory(void)
{
	QString task_directory = QFileDialog::getExistingDirectory(this, QString::fromLocal8Bit("Выброр каталога"), "", QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
	if (!task_directory.isEmpty()) {
		ui.DirectoryLineEdit->setText(task_directory);
	}

	//QFileDialog file_dialog;
	//file_dialog.setFileMode(QFileDialog::Directory);
	//file_dialog.setViewMode(QFileDialog::List);
	//file_dialog.setOption(QFileDialog::ShowDirsOnly, true);
	//file_dialog.setWindowTitle(QString::fromLocal8Bit("Выброр каталога"));
	//if (QDialog::Accepted == file_dialog.exec()) {
	//	ui.DirectoryLineEdit->setText(file_dialog.directory().absolutePath());
	//}
	this->activateWindow();
}
