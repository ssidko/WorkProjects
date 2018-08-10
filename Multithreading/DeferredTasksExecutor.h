#ifndef _DEFERRED_TASKS_EXECUTOR_H
#define _DEFERRED_TASKS_EXECUTOR_H

#include <vector>
#include <thread>
#include <atomic>

#include "DeferredTasksExecutor.h"
#include "TSQueue.h"

void trace(const std::string str);

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
	std::atomic<int> in_progress;	

	void worker_func(size_t id);
	bool next_task(Task &task);
	void terminate_and_join_all_threads(void);
};

#endif // _DEFERRED_TASKS_EXECUTOR_H
