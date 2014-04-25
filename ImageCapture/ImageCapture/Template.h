#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <QString>
#include <QList>

#define TEMPLATES_DIRECTORY_NAME		"templates"
#define TEMPLATES_EXTENSION				"*.xml"

typedef struct _BUTTON {
	QString name;
	QString command;
} BUTTON;

typedef struct _SECTION {
	QString name;
	QList<QString> pictures;
} SECTION;

class Template
{
private:
	QString name;
	QString description;
	QString file_path;
	QList<BUTTON> buttons;
	QList<SECTION> sections;
public:
	Template(void);
	~Template(void);
	static QStringList AllTemplates(void);
	static QString TemplateName(QString &template_path);
	static QString TemplateDescription(QString &template_path);
	bool Initialize(const QString &template_path);
	QString Name(void) const {return name;}
	QString Description(void) const {return description;}
	QString FilePath(void) const {return file_path;}
	const QList<BUTTON> *Buttons(void) const {return &buttons;}
	const QList<SECTION> *Sections(void) const {return &sections;}
};

#endif

