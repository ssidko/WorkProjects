#ifndef TASK_H
#define TASK_H

#include <QString>

class Task
{
private:
	QString name;
	QString description;
	QString directory;
public:
	Task(void);
	~Task(void);
	bool Create(const QString &task_name, const QString &path);
	QString Description(void) {return description;}
	void SetDescription(const QString &task_description) {description = task_description;}
};

#endif

