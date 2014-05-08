#include "Task.h"
#include <QDir>
#include <QFile>
#include <QTextCodec>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

Task::Task(void)
{
}

Task::~Task(void)
{
}

bool Task::Create(const QString &task_name, const QString &path)
{
	QDir dir;
	QString task_directory = path + "/" + task_name;
	QString resource_directory = task_directory + "/" + TASK_RESOURCES_DIRECTORY;
	if (!dir.exists(task_directory) && dir.mkpath(task_directory) && dir.mkpath(resource_directory)) {
		name = task_name;
		directory = task_directory;
		res_directory = resource_directory;
		return true;
	}
	return false;
}

bool Task::AddTemplate(const QString &template_path)
{
	Template *t = new Template();
	if (t && t->Initialize(template_path)) {
		return AddTemplate(t);
	} else {
		return false;
	}
}

bool Task::AddTemplate(Template *t)
{
	if (t) {
		templates.push_back(t);
		return true;
	} else {
		return false;
	}
}

bool Task::Save(void)
{
	QString file_name = directory + "/" + name + TASK_FILE_EXTENSION;
	QFile file(file_name);
	if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QXmlStreamWriter xml(&file);
		xml.setCodec(QTextCodec::codecForName("Windows-1251"));
		xml.setAutoFormatting(true);
		xml.writeStartDocument();

		xml.writeStartElement("Task");
		xml.writeAttribute("Name", name);
		xml.writeAttribute("Description", description);
		xml.writeAttribute("Directory", directory);
		
		foreach (const Template *templ, templates) {
			xml.writeStartElement("Template");
			xml.writeAttribute("Path", templ->FilePath());
			foreach (const SECTION *section, templ->Sections()) {
				if (section->pictures.size()) {
					xml.writeStartElement("Section");
					xml.writeAttribute("Name", section->name);
					foreach (const QString &pict_name, section->pictures) {
						xml.writeStartElement("Picture");
						xml.writeAttribute("Name", pict_name);
						xml.writeEndElement(); // "Picture"
					}
					xml.writeEndElement(); // "Section"
				}
			}
			xml.writeEndElement(); // "Template"
		}

		xml.writeEndElement(); // "Task"		

		xml.writeEndDocument();
		file.close();
		return true;
	}
	return false;
}
