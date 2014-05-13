#include "Task.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDateTime>
#include <QTextCodec>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QTextStream>

Task::Task(void)
{
}

Task::~Task(void)
{
	Close();
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
		creation_time = QDateTime::currentDateTime().toString();
		return true;
	}
	return false;
}

bool Task::Open(const QString &task_file)
{
	Close();
	QFile file(task_file);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QXmlStreamReader xml(&file);
		while(xml.readNextStartElement()) {
			if (xml.name() == "Task") {
				name = xml.attributes().value("Name").toString();
				description = xml.attributes().value("Description").toString();
				creation_time = xml.attributes().value("Timestamp").toString();
				directory = QFileInfo(task_file).absolutePath();
				res_directory = directory + "/" + TASK_RESOURCES_DIRECTORY;
			} else if (xml.name() == "Template") {
				QString template_path = xml.attributes().value("Path").toString();
				Template *t = new Template();
				if (!t) {
					Close();
					return false;
				} 
				if (t->Initialize(template_path)) {
					templates.push_back(t);
					while(xml.readNextStartElement()) {
						if (xml.name() == "Section") {
							QString section_name = xml.attributes().value("Name").toString();
							while(xml.readNextStartElement()) {
								if (xml.name() == "Picture") {
									QString picture_name = xml.attributes().value("Name").toString();
									t->AddPicture(section_name, picture_name);
								} 
								xml.skipCurrentElement();
							}
						} else {
							xml.skipCurrentElement();
						}
					}
				} else {
					xml.skipCurrentElement();
				}
			} else {
				xml.skipCurrentElement();
			}
		}
		return true;
	}
	return false;
}

void Task::Close( void )
{
	name = "";
	description = "";
	directory = "";
	res_directory = "";
	creation_time = "";
	control_unit_id = "";
	foreach (const Template *t, templates) {
		if (t) {
			delete t;
		}
	}
	templates.clear();
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
	QString file_name = directory + "/" + name + "." + TASK_FILE_EXTENSION;
	QFile file(file_name);
	if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QXmlStreamWriter xml(&file);
		xml.setCodec(QTextCodec::codecForName("Windows-1251"));
		xml.setAutoFormatting(true);
		xml.writeStartDocument();

		xml.writeStartElement("Task");
		xml.writeAttribute("Name", name);
		xml.writeAttribute("Description", description);
		xml.writeAttribute("Timestamp", creation_time);

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

bool Task::CreateReport(void)
{
	int counter = 0;
	QFile file(directory + "/" +QString::fromLocal8Bit(REPORT_FILE_NAME));
	if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QTextStream out(&file);

		out << "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">";
		out << "<html>" << " <head>";
		out << QString::fromLocal8Bit("<title>ОТЧЕТ</title>");
		out << "</head>";
		
		out << "</body>";

		foreach (Template *t, templates) {
			out << "<TABLE BORDER=\"1\" cellpadding=\"4\" cellspacing=\"0\">";
			out << QString::fromLocal8Bit("<CAPTION><h2>Сводка</h2></CAPTION>");
			out << QString::fromLocal8Bit("<TR bgcolor=\"#ddd\"><TD><p><b>Комплекс</p></b></TD><TD><p>Игроскан-01 серийный  № ") << control_unit_id << "</TD></TR>";		
			out << QString::fromLocal8Bit("<TR><TD><p><b>Дата начала извлечения</p></b></TD><TD><p>") << creation_time << "</p></TD></TR>";		
			out << QString::fromLocal8Bit("<TR bgcolor=\"#ddd\"><TD><p><b>Автомат</p></b></TD><TD><p>") << t->Name() + ", " + t->Description() << "</p></TD></TR>";		
			out << QString::fromLocal8Bit("<TR><TD><p><b>Задача</p></b></TD><TD><p>") << name << "</p></TD></TR>";		
			out << "</TABLE>";

			out << QString::fromLocal8Bit("<h2>Данные</h2>");

			foreach (SECTION *section, t->Sections()) {
				out << "<h3>" << section->name << "</h3>";
				counter = 0;
				foreach (QString picture_name, section->pictures) {
					out << QString::fromLocal8Bit("<div>");
					out << "<p><img src=" << ".\\" << TASK_RESOURCES_DIRECTORY << "\\" << picture_name << "></p>";
					out << QString::fromLocal8Bit("<p>Рис. ") << ++counter << "."  << "</p></div>";	
				}
			}
		}

		out << "</body>";
		out << "</html>";
		out.flush();

		return true;
	} else {
		return false;
	}
}
