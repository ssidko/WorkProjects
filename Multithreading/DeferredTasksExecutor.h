#ifndef _DEFERRED_TASKS_EXECUTOR_H
#define _DEFERRED_TASKS_EXECUTOR_H

#include <vector>
#include <memory>
#include <thread>
#include <atomic>

#include "DeferredTasksExecutor.h"
#include "TSQueue.h"

void trace(const std::string str);

using TaskFunction = std::function<void()>;

enum {
	sleep_for_next_try_usec = 100,
};

enum class TaskStatus {
	not_in_queue,
	wait_for_precondition,
	in_queue,
	processing,
	done
};

const char *task_status_to_string(TaskStatus status);

class DeferredTasksExecutor;

class DeferredTask
{
	friend DeferredTasksExecutor;
public:
	DeferredTask(TaskFunction task_function);

	DeferredTask(const DeferredTask &task) = delete;
	DeferredTask & operator =(const DeferredTask &task) = delete;

	TaskStatus status(void);	
	void wait_for_done();
	bool is_done(void);
	void operator ()(void);

private:
	TaskFunction function;
	std::mutex mtx;
	TaskStatus task_status;

	void set_status(TaskStatus new_status);
};

class DeferredTasksExecutor
{	
public:
	static DeferredTasksExecutor & get_instance(void);
	size_t pool_size(void);
	std::shared_ptr<DeferredTask> add_task(TaskFunction task_function);
	void wait_for_all_done(void);

private:
	std::atomic<bool> terminate;
	std::atomic<int> tasks_in_progress;
	TSQueue<std::shared_ptr<DeferredTask>> tasks;
	std::vector<std::thread> pool;
	
	DeferredTasksExecutor();
	~DeferredTasksExecutor();
	DeferredTasksExecutor(const DeferredTasksExecutor &) = delete;
	DeferredTasksExecutor(const DeferredTasksExecutor &&) = delete;
	DeferredTasksExecutor & operator=(const DeferredTasksExecutor &) = delete;
	DeferredTasksExecutor & operator=(const DeferredTasksExecutor &&) = delete;

	void worker_thread(size_t id);
	bool next_task(std::shared_ptr<DeferredTask> &task);
	void terminate_and_join_all_threads(void);
};

#endif // _DEFERRED_TASKS_EXECUTOR_H
