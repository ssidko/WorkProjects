#ifndef _DEFERRED_TASKS_EXECUTOR_H
#define _DEFERRED_TASKS_EXECUTOR_H

#include "DeferredTasksExecutor.h"

#include<vector>
#include<thread>
#include<future>

#include "trace.h"
#include "TSQueue.h"

using Task = std::function<void()>;

class DeferredTask
{
public:
	DeferredTask(std::function<void()> fn);
	void operator ()(void) {
	
	}
private:
	std::function<void()> func;
};

class DeferredTasksExecutor
{
	friend void WorkerFunc(DeferredTasksExecutor &tsk_executor, size_t id);
public:
	DeferredTasksExecutor();
	~DeferredTasksExecutor();

	void AddTask(Task task)
	{
		tasks.Push(task);
		std::lock_guard<std::mutex> lock(mtx);
		++added;
	}
private:
	std::vector<std::thread> pool;
	TSQueue<Task> tasks;
	/*volatile*/ bool terminate;

	std::mutex mtx;
	size_t added;
	size_t executed;
};

#endif // _DEFERRED_TASKS_EXECUTOR_H
