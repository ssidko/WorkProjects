#include "mainwindow.h"
#include <QFileInfo>

//#define CAMERA_NAME							"USB 2861 Video"
//#define CAMERA_NAME							"iLook 300"
#define CAMERA_NAME								"ASUS USB2.0 Webcam"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), task(NULL), camera(NULL), image_capture(NULL)
{
	ui.setupUi(this);
	Initialize();
	connect(ui.ScreenshotButton, SIGNAL(clicked(bool)), SLOT(TakeScreenshot(void)));
	connect(ui.CreateAction, SIGNAL(triggered()), SLOT(CreateNewTask()));
	ui.CreateAction->setShortcut(tr("Ctrl+N"));
	connect(ui.SaveAction, SIGNAL(triggered()), SLOT(SaveTask()));
	ui.SaveAction->setShortcut(tr("Ctrl+S"));
	ui.ButtonsGroupBox->setAlignment(Qt::AlignHCenter);
	connect(ui.TaskTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(CheckSelection(QTreeWidgetItem*, QTreeWidgetItem*)));
	connect(ui.TaskTreeWidget, SIGNAL(itemSelectionChanged()), SLOT(ChangeSection()));
}

MainWindow::~MainWindow()
{
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
	if (control_unit.Open()) {
		if (dlg.exec() == QDialog::Accepted) {
			SetTask(dlg.NewTask());
			return true;
		}
	} 
	return false;
}

bool MainWindow::SaveTask( void )
{
	if (task) {
		return task->Save();
	} else {
		return false;	
	}
}

QString MainWindow::MakeTemplateItemName(const Template &t)
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

	if (new_task) {
		task = new_task;
		setWindowTitle(QString::fromLocal8Bit("������: ") + task->Name());
		ui.TaskTreeWidget->setColumnCount(1);
		ui.TaskTreeWidget->setHeaderLabel(QString::fromLocal8Bit("������"));

		QTreeWidgetItem *task_item = new QTreeWidgetItem(ui.TaskTreeWidget);
		QFont segoe_font("Segoe UI", 9, QFont::Bold);
		task_item->setFont(0, segoe_font);
		task_item->setText(0, task->Name());
		task_item->setFlags(Qt::ItemIsEnabled);
		task_item->setExpanded(true);

		QTreeWidgetItem *task_description = new QTreeWidgetItem(task_item);
		task_description->setText(0, QString::fromLocal8Bit("��������: ") + task->Description());
		task_description->setFlags(Qt::ItemIsEnabled);

		foreach(const Template *t, task->Templates()) {
			QTreeWidgetItem *template_item = new QTreeWidgetItem(task_item);
			QVariant template_data(QString("template"));
			template_item->setText(0, MakeTemplateItemName(*t));
			template_item->setData(0, Qt::AccessibleDescriptionRole, template_data);
			template_item->setFlags(Qt::ItemIsEnabled);
			template_item->setExpanded(true);
			foreach (const SECTION *section, t->Sections()) {
				QTreeWidgetItem *section_item = new QTreeWidgetItem(template_item);
				QVariant section_data(QString("section"));
				section_item->setText(0, section->name);
				section_item->setData(0, Qt::AccessibleDescriptionRole, section_data);
			}
			ui.TaskTreeWidget->setCurrentItem(template_item->child(0));
			ui.ScreenGroupBox->setTitle(template_item->child(0)->text(0));
		}
		CreateButtons(*(task->Templates())[0]);
	}
}

void MainWindow::TakeScreenshot(void)
{
	static unsigned int counter = 0;
	if (task) {
		QString picture_name = task->ResDirectory() + "/" + QString::number(counter, 10) + ".jpg";
		camera->searchAndLock();
		image_capture->capture(picture_name);
		camera->unlock();
		counter++;
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
		return;
	}

	if (current_item->data(0, Qt::AccessibleDescriptionRole).toString() != "section") {
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
		if (template_name == MakeTemplateItemName(*t)) {
			return t;
		}
	}
	return NULL;
}
