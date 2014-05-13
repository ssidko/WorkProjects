#include "mainwindow.h"
#include <QFileInfo>
#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>

#define CAMERA_NAME								"USB 2861 Video"
//#define CAMERA_NAME							"iLook 300"
//#define CAMERA_NAME							"ASUS USB2.0 Webcam"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), task(NULL), camera(NULL), image_capture(NULL)
{
	ui.setupUi(this);
	connect(ui.ScreenshotButton, SIGNAL(clicked(bool)), SLOT(TakeScreenshot(void)));
	ui.ScreenshotButton->setDefault(true);
	connect(ui.TaskTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(CheckSelection(QTreeWidgetItem*, QTreeWidgetItem*)));
	connect(ui.TaskTreeWidget, SIGNAL(itemSelectionChanged()), SLOT(ChangeSection()));
	Initialize();
	InitializeActions();
	UpdateWindowTitle();

	ui.statusBar->setSizeGripEnabled(false);
	//ui.statusBar->showMessage(QString::fromLocal8Bit("Усё готово!"));
}

MainWindow::~MainWindow()
{
	if (camera) {
		delete camera;
	}
}

bool MainWindow::Initialize()
{
	QString description;
	foreach(const QByteArray &device_name, QCamera::availableDevices()) {
		description = QCamera::deviceDescription(device_name);
		if (description == CAMERA_NAME) {
			camera = new QCamera(device_name);
			if (camera) {
				camera->setViewfinder((QVideoWidget *)ui.ScreenWidget);
				camera->setCaptureMode(QCamera::CaptureStillImage);
				image_capture = new QCameraImageCapture(camera);
				image_capture->setCaptureDestination(QCameraImageCapture::CaptureToFile);
				connect(image_capture, SIGNAL(imageSaved(int, const QString&)), SLOT(AddScreenshot(int, const QString&)));

				QImageEncoderSettings image_settings;
				image_settings.setCodec("image/jpeg");
				image_settings.setResolution(640, 480);
				image_capture->setEncodingSettings(image_settings);
	
				camera->start();
			} else {
				return false;
			}
		}
	}
	return true;
}

void MainWindow::InitializeActions(void)
{
	connect(ui.CreateAction, SIGNAL(triggered()), SLOT(CreateNewTask()));
	ui.CreateAction->setShortcut(tr("Ctrl+N"));
	ui.CreateAction->setIcon(QIcon(":/MainWindow/images/new.png"));
	ui.CreateAction->setStatusTip(QString::fromLocal8Bit("Создать новую задачу"));

	connect(ui.OpenAction, SIGNAL(triggered()), SLOT(OpenTask()));
	ui.OpenAction->setShortcut(tr("Ctrl+O"));
	ui.OpenAction->setIcon(QIcon(":/MainWindow/images/open.png"));
	ui.OpenAction->setStatusTip(QString::fromLocal8Bit("Открыть существующую задачу"));

	connect(ui.SaveAction, SIGNAL(triggered()), SLOT(SaveTask()));
	ui.SaveAction->setShortcut(tr("Ctrl+S"));
	ui.SaveAction->setIcon(QIcon(":/MainWindow/images/save.png"));
	ui.SaveAction->setStatusTip(QString::fromLocal8Bit("Сохранить текущую задачу"));

	ui.PropertiesAction->setIcon(QIcon(":/MainWindow/images/properties.png"));
	ui.PropertiesAction->setStatusTip(QString::fromLocal8Bit("Посмотреть/отредактировать свойства текущей задачи"));

	connect(ui.CreateReportAction, SIGNAL(triggered()), SLOT(CreateReport()));
	ui.CreateReportAction->setIcon(QIcon(":/MainWindow/images/report-1.png"));
	ui.CreateReportAction->setStatusTip(QString::fromLocal8Bit("Сформировать отчет по текущей задаче"));

	connect(ui.AboutAction, SIGNAL(triggered()), SLOT(ShowAboutDialog()));

	connect(ui.HelpAction, SIGNAL(triggered()), SLOT(ShowHelp()));
	ui.HelpAction->setIcon(QIcon(":/MainWindow/images/addtab.png"));
}

void MainWindow::UpdateWindowTitle()
{
	QString title;
	if (task) {
		title = task->Name() + " - ";
	}
	setWindowTitle(title + QString::fromLocal8Bit(MAIN_WINDOW_TITLE));
}


bool MainWindow::CreateButtons(const Template &t)
{
	if (task) {
		DestroyButtons();
		foreach (const BUTTON *button, t.Buttons()) {
			CmdButton *cmd_button = new CmdButton(button->command, ui.ButtonsGroupBox);
			if (cmd_button) {
				buttons.append(cmd_button);
				cmd_button->setText(button->name);
				ui.ButtonsHLayout->addWidget(cmd_button);
				cmd_button->show();
				connect(cmd_button, SIGNAL(SendCommand(QString)), SLOT(SendCommand(QString)));
			}
		}
	}
	return false;
}

void MainWindow::DestroyButtons(void)
{
	foreach (CmdButton *button, buttons) {
		delete button;
	}
	buttons.clear();
}

bool MainWindow::CreateNewTask(void)
{
	NewTaskDialog dlg;
	if (dlg.exec() == QDialog::Accepted) {
		CloseTask();
		SetTask(dlg.NewTask());
		return true;
	}
	return false;
}

bool MainWindow::SaveTask( void )
{
	if (task) {
		if (task->Save()) {
			QMessageBox::information(this, this->windowTitle(), QString::fromLocal8Bit("Задача успешно сохранена"));
			return true;
		} else {
			QMessageBox::warning(this, this->windowTitle(), QString::fromLocal8Bit("При сохранении задачи произошла ошибка"));
			return false;
		}
	} else {
		return false;
	}		
}

QString MainWindow::ComposeTemplateItemName(const Template &t)
{
	QString item_name;
	item_name = t.Name() + ", " + t.Description();
	return item_name;
}

void MainWindow::SetTask(Task *new_task)
{
	//
	// SaveCurrentTask();
	//
	
	if (!control_unit.IsOpened()) {
		control_unit.IsAvailable();
		control_unit.Open();
	}

	if (new_task) {
		task = new_task;
		setWindowTitle(QString::fromLocal8Bit("Задача: ") + task->Name());
		ui.TaskTreeWidget->setColumnCount(1);
		ui.TaskTreeWidget->setHeaderLabel(QString::fromLocal8Bit("Задача"));

		QTreeWidgetItem *task_item = new QTreeWidgetItem(ui.TaskTreeWidget);
		QFont segoe_font("Segoe UI", 9, QFont::Bold);
		task_item->setFont(0, segoe_font);
		task_item->setText(0, task->Name());
		task_item->setIcon(0, QIcon(":/MainWindow/images/task.png"));
		task_item->setFlags(Qt::ItemIsEnabled);
		task_item->setExpanded(true);

		QTreeWidgetItem *task_description = new QTreeWidgetItem(task_item);
		task_description->setText(0, QString::fromLocal8Bit("Описание: ") + task->Description());
		task_description->setFlags(Qt::ItemIsEnabled);

		foreach(const Template *t, task->Templates()) {
			QTreeWidgetItem *template_item = new QTreeWidgetItem(task_item);
			QVariant template_data(QString("template"));
			template_item->setText(0, ComposeTemplateItemName(*t));
			template_item->setData(0, Qt::AccessibleDescriptionRole, template_data);
			template_item->setFlags(Qt::ItemIsEnabled);
			template_item->setExpanded(true);
			foreach (const SECTION *section, t->Sections()) {
				QTreeWidgetItem *section_item = new QTreeWidgetItem(template_item);
				QVariant section_data(QString("section"));
				section_item->setText(0, section->name);
				section_item->setData(0, Qt::AccessibleDescriptionRole, section_data);
				foreach (QString picture, section->pictures) {
					QTreeWidgetItem *picture_item = new QTreeWidgetItem(section_item);
					picture_item->setText(0, picture);
					picture_item->setData(0, Qt::AccessibleDescriptionRole, QVariant(QString("picture")));
					picture_item->setFlags(Qt::NoItemFlags);
				}
				section_item->setExpanded(true);
			}
			ui.TaskTreeWidget->setCurrentItem(template_item->child(0));
			ui.ScreenGroupBox->setTitle(template_item->child(0)->text(0));
		}
		CreateButtons(*(task->Templates())[0]);
		UpdateWindowTitle();
	}
}

void MainWindow::TakeScreenshot(void)
{
	static unsigned int counter = 0;
	if (task && camera) {
		QString picture_name = task->ResDirectory() + "/" + QString::number(counter, 10)/* + ".jpg"*/;
		camera->searchAndLock();
		image_capture->capture(picture_name);
		camera->unlock();
		counter++;
		ui.ScreenshotButton->setEnabled(false);
	}
}

bool MainWindow::SendCommand(QString command)
{
	if (control_unit.IsOpened()) {
		control_unit.SenCommand(command);
		return true;
	}
	return false;
}

void MainWindow::CheckSelection(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
	if (current && previous) {
		QString current_name = current->text(0);
		QString prev_name = previous->text(0);
		QVariant data = current->data(0, Qt::AccessibleDescriptionRole);
		if (data.toString() != "section") {
			ui.TaskTreeWidget->setCurrentItem(previous);
			//previous->setSelected(true);
		} else {
			//previous->setSelected(false);
		}
	}
}

void MainWindow::ChangeSection(void)
{
	QTreeWidgetItem *item = ui.TaskTreeWidget->currentItem();
	if (item) {
		QVariant data = item->data(0, Qt::AccessibleDescriptionRole);
		if (data.toString() == "section") {
			ui.ScreenGroupBox->setTitle(item->text(0));
			current_item = item;
		}
	}
}

void MainWindow::AddScreenshot(int id, const QString &file_path)
{
	QTreeWidgetItem *current_item = ui.TaskTreeWidget->currentItem();
	if (!current_item) {
		ui.ScreenshotButton->setEnabled(true);
		return;
	}

	if (current_item->data(0, Qt::AccessibleDescriptionRole).toString() != "section") {
		ui.ScreenshotButton->setEnabled(true);
		return;			
	}

	QFileInfo file_info(file_path);
	QString item_name = file_info.fileName();
	QString section_name = current_item->text(0);
	QTreeWidgetItem *picture_item = new QTreeWidgetItem(current_item);
	picture_item->setText(0, item_name);
	picture_item->setData(0, Qt::AccessibleDescriptionRole, QVariant(QString("picture")));
	picture_item->setFlags(Qt::NoItemFlags);
	ui.TaskTreeWidget->currentItem()->setExpanded(true);

	Template *t = CurrentTemplate();
	if (t) {
		foreach (SECTION *section, t->Sections()) {
			if (section->name == section_name) {
				section->pictures.push_back(item_name);
			}
		}
	}
	ui.ScreenshotButton->setEnabled(true);
}

Template *MainWindow::CurrentTemplate(void)
{
	if (!task) {
		return NULL;
	}
	
	QString template_name = "";
	QTreeWidgetItem *current_item = ui.TaskTreeWidget->currentItem();
	if (!current_item) {
		return NULL;
	}
	QVariant data = current_item->data(0, Qt::AccessibleDescriptionRole);
	if (data.toString() == "section") {
		template_name = current_item->parent()->text(0);		
	} else if (data.toString() == "template") {
		template_name = current_item->text(0);
	} else {
		return NULL;
	}

	foreach (Template *t, task->Templates()) {
		if (template_name == ComposeTemplateItemName(*t)) {
			return t;
		}
	}
	return NULL;
}

void MainWindow::ShowAboutDialog(void)
{
	QString str;
	str += QString::fromLocal8Bit("<p><b style='mso-bidi-font-weight:normal'><span lang=RU>Аппаратно-программный комплекс «Игроскан-01»<o:p></o:p></span></b></p>");
	str += QString::fromLocal8Bit("<p><span lang=RU>Исследование устройств, предназначенных для проведения азартных игр<o:p></o:p></span></p>");
	str += QString::fromLocal8Bit("<p><span lang=RU>Версия 0.1<o:p></o:p></span></p>");
	str += QString::fromLocal8Bit("<p><span lang=RU>(с) Лаборатория компьютерной криминалистики, 2014<o:p></o:p></span></p>");
	str += QString::fromLocal8Bit("<p><a href=\"www.forensictools.com.ua\">www.forensictools.com.ua</a></p>");
	QMessageBox::about(this, QString::fromLocal8Bit("О программе ") + QString::fromLocal8Bit(MAIN_WINDOW_TITLE), str);
}

void MainWindow::ShowHelp( void )
{
	QMessageBox::information(this, QString::fromLocal8Bit("Помощь"), QString::fromLocal8Bit("Ждите в следующих релизах"));
}

bool MainWindow::OpenTask(void)
{
	QString filter = QString("*.") + TASK_FILE_EXTENSION;
	QString task_file = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("Открыть задачу"), QSettings().value(SETTINGS_TASK_DIRECTORY).toString(), filter);
	if (!task_file.isEmpty()) {
		Task *new_task = new Task();
		if (new_task) {
			if (new_task->Open(task_file)) {
				CloseTask();
				SetTask(new_task);
				return true;
			} else {
				delete new_task;
				QMessageBox::warning(this, this->windowTitle(), QString::fromLocal8Bit("Открыть задачу не удалось"));
				return false;
			}
		}
	}
	return false;
}

void MainWindow::CloseTask(void)
{
	if (task) {
		delete task;
		task = NULL;
	}
	ui.TaskTreeWidget->clear();
}

bool MainWindow::CreateReport(void)
{
	if (task) {
		if (task->CreateReport()) {
			QMessageBox::information(this, this->windowTitle(), QString::fromLocal8Bit("Отчет успешно сформирован"));
			return true;
		} else {
			QMessageBox::warning(this, this->windowTitle(), QString::fromLocal8Bit("Сформировать отчет не удалось"));
		}
	}
	return false;
}