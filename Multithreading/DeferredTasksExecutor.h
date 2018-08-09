#ifndef _DEFERRED_TASKS_EXECUTOR_H
#define _DEFERRED_TASKS_EXECUTOR_H

#include "DeferredTasksExecutor.h"

#include<vector>
#include<thread>
#include<future>

#include "TSQueue.h"

using Task = void(*)(void);

class DeferredTasksExecutor
{
public:
	DeferredTasksExecutor();
	~DeferredTasksExecutor();

	std::future<void> AddTask(Task task)
	{
		tasks.Push(task);
		return std::future<void>();
	}
private:
	std::vector<std::thread> pool;
	TSQueue<Task> tasks;
};

#endif // _DEFERRED_TASKS_EXECUTOR_H
