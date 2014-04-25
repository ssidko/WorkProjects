#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QList>
#include "Template.h"

class Task
{
private:
	QString name;
	QString description;
	QString directory;
	QString res_directory;
	QList<Template> templates;
public:
	Task(void);
	~Task(void);
	bool Create(const QString &task_name, const QString &path);
	QString Name(void) const {return name;}
	QString Description(void) const {return description;}
	void SetDescription(const QString &task_description) {description = task_description;}
	bool AddTemplate(const QString &template_path);
	void AddTemplate(const Template &t) {templates.push_back(t);}
	const QList<Template> *Templates(void) {return &templates;}
	bool Save(void);
};

#endif

