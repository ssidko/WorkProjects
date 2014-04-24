#include "Task.h"
#include <QDir>

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
	if (dir.mkpath(task_directory)) {
		directory = task_directory;
		name = task_name;
	}
	return true;
}
