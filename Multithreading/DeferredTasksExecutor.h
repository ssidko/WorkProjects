#ifndef _DEFERRED_TASKS_EXECUTOR_H
#define _DEFERRED_TASKS_EXECUTOR_H

#include <vector>
#include <thread>

#include "DeferredTasksExecutor.h"
#include "TSQueue.h"

using Task = std::function<void()>;

enum class TaskStatus {
	in_queue,
	processing,
	done
};

//class DeferredTask
//{
//public:
//	DeferredTask(std::function<void()> fn);
//	void operator ()(void) {
//	
//	}
//private:
//	std::function<void()> func;
//};

class DeferredTasksExecutor
{	
public:
	DeferredTasksExecutor();
	~DeferredTasksExecutor();

	void add_task(Task task);
	void wait_for_complete_all_tasks(void);

private:
	std::atomic<bool> terminate;
	TSQueue<Task> tasks;
	std::vector<std::thread> pool;

	void worker_func(size_t id);
	void terminate_and_join_all_threads(void);
};

#endif // _DEFERRED_TASKS_EXECUTOR_H
