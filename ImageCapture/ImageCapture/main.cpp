#include "mainwindow.h"
#include <QtWidgets/QApplication>


#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QTextCodec>
#include <QCamera>

#include "NewCaseDialog.h"

void TestXML(void)
{
	QFile file("test_XML.xml");
	if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QXmlStreamWriter xml(&file);
		xml.setCodec(QTextCodec::codecForName("Windows-1251"));
		xml.setAutoFormatting(true);
		xml.writeStartDocument();

		xml.writeStartElement("Buttons");

		xml.writeStartElement("Button");
		xml.writeAttribute("Name", "Button 1");
		xml.writeAttribute("BindPin", "Pin1");
		xml.writeEndElement();

		xml.writeStartElement("Button");
		xml.writeAttribute("Name", "Button 2");
		xml.writeAttribute("BindPin", "Pin2");
		xml.writeEndElement();

		xml.writeStartElement("Button");
		xml.writeAttribute("Name", "Button 3");
		xml.writeAttribute("BindPin", "Pin3");
		xml.writeEndElement();

		xml.writeEndElement(); // Buttons

		xml.writeEndDocument();
		file.close();
	}
	int x = 0;
}

bool CreateTestTemplate(QString &file_name)
{
	QFile file(file_name);
	if (file.open(QIODevice::ReadWrite | QIODevice::Text)) {
		QXmlStreamWriter xml(&file);
		xml.setCodec(QTextCodec::codecForName("Windows-1251"));
		xml.setAutoFormatting(true);
		xml.writeStartDocument();

		xml.writeStartElement("Template");
		xml.writeAttribute("Name", QString::fromLocal8Bit("Игровой автомат №1"));
		xml.writeAttribute("Description", QString::fromLocal8Bit("Однорукий бандит, тип 1"));

		xml.writeStartElement("Buttons");

		xml.writeStartElement("Button");
		xml.writeAttribute("Name", "Button 1");
		xml.writeAttribute("Command", "Pin1");
		xml.writeEndElement();

		xml.writeStartElement("Button");
		xml.writeAttribute("Name", "Button 2");
		xml.writeAttribute("Command", "Pin2");
		xml.writeEndElement();

		xml.writeStartElement("Button");
		xml.writeAttribute("Name", "Button 3");
		xml.writeAttribute("Command", "Pin3");
		xml.writeEndElement();

		xml.writeEndElement(); // Buttons

		xml.writeStartElement("Sections");

		xml.writeStartElement("Section");
		xml.writeAttribute("Name", QString::fromLocal8Bit("Главный экран"));
		xml.writeEndElement();

		xml.writeStartElement("Section");
		xml.writeAttribute("Name", QString::fromLocal8Bit("Экран статистики"));
		xml.writeEndElement();

		xml.writeStartElement("Section");
		xml.writeAttribute("Name", QString::fromLocal8Bit("Разное"));
		xml.writeEndElement();

		xml.writeEndElement(); // Sections
		
		xml.writeEndElement(); // Template

		xml.writeEndDocument();
		file.close();
		return true;
	}
	return false;
}

bool ReadTestTemplate(QString &file_name)
{
	QFile file(file_name);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QXmlStreamReader xml(&file);
		QXmlStreamAttributes attr;
		QXmlStreamReader::TokenType token;
		QString str;
		while(xml.readNextStartElement()) {
			if (xml.name() == "Template") {
				str = xml.attributes().value("Name").toString();
			} else if (xml.name() == "Button") {
				str = xml.attributes().value("Name").toString();
				str = xml.attributes().value("BindPin").toString();
			} else {
				str = xml.name().toString();
			}
		}	
		file.close();
		return true;
	}
	return false; 
}

#include "windows.h"
#include "TCHAR.h"

int COMTest(void)
{
	HANDLE com = NULL;
	DWORD err = 0;
	com = ::CreateFileA(_T("\\\\.\\COM12"), GENERIC_READ|GENERIC_WRITE, 0/*exclusive access*/, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (com == INVALID_HANDLE_VALUE) return -1;
	
	DCB com_param;
	memset(&com_param, 0x00, sizeof(com_param));
	if (!GetCommState(com, &com_param)) {
		err = ::GetLastError();
		return -1;
	}
	com_param.BaudRate = CBR_9600;
	com_param.ByteSize = 8;
	com_param.StopBits = ONESTOPBIT;
	com_param.Parity = NOPARITY;
	if (!SetCommState(com, &com_param)) {
		err = ::GetLastError();
		return -1;
	}

	char *cmd = "echo\n";
	int len = strlen(cmd);
	DWORD rw = 0;
	if (!WriteFile(com, cmd, strlen(cmd), &rw, NULL)) {
		err = ::GetLastError();
		return -1;
	}
	
	::CloseHandle(com);
	return 0;
}

#include "WinDevicesManager.h"
#include "WriterWindow.h"

#include "Template.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	NewCaseDialog dlg;
	MainWindow w;
	
	dlg.exec();
	w.show();
	w.activateWindow();
	//if (QDialog::Accepted == dlg.result()) {
	//	w.show();
	//	w.activateWindow();
	//}
	return a.exec();
}
