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
};

#endif

