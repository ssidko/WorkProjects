#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <QString>
#include <QList>

#define TEMPLATES_DIRECTORY_NAME		"templates"
#define TEMPLATES_EXTENSION				"*.xml"

typedef struct _TEMPLATE_BUTTON {
	QString name;
	QString command;
} TEMPLATE_BUTTON;

typedef struct _TEMPLATE_SECTION {
	QString name;
	QList<QString> pictures;
} TEMPLATE_SECTION;

class Template
{
private:
	QString name;
	QString description;
	QString file_path;
	QList<TEMPLATE_BUTTON> buttons;
	QList<TEMPLATE_SECTION> sections;
public:
	Template(void);
	~Template(void);
	static QStringList AllTemplates(void);
	static QString TemplateName(QString &template_path);
	static QString TemplateDescription(QString &template_path);
	bool Initialize(const QString &template_path);
	QString Name(void) {return name;}
	QString Description(void) {return description;}
	QString FilePath(void) {return file_path;}
	const QList<TEMPLATE_BUTTON> *Buttons(void) {return &buttons;}
	const QList<TEMPLATE_SECTION> *Sections(void) {return &sections;}
};

#endif

