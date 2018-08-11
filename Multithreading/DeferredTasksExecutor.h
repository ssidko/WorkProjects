#ifndef _DEFERRED_TASKS_EXECUTOR_H
#define _DEFERRED_TASKS_EXECUTOR_H

#include <vector>
#include <list>
#include <memory>
#include <thread>
#include <atomic>

#include "DeferredTasksExecutor.h"
#include "TSQueue.h"

void trace(const std::string str);

using task_function_t = std::function<void()>;
using precondition_t = std::function<bool()>;

enum class TaskStatus {
	not_in_queue,
	wait_for_precondition,
	in_queue,
	processing,
	done
};

const char *task_status_to_string(TaskStatus status);

class DeferredTasksExecutor;

class Task
{
	friend DeferredTasksExecutor;
public:
	Task(task_function_t task_function);

	Task(const Task &) = delete;
	Task(const Task &&) = delete;
	Task & operator =(const Task &) = delete;
	Task & operator =(const Task &&) = delete;

	TaskStatus status(void);	
	void wait_for_done();
	bool is_done(void);
	void operator ()(void);

private:
	task_function_t function;
	std::atomic<int> task_status;

	void set_status(TaskStatus new_status);
};

class DeferredTasksExecutor
{	
public:
	static DeferredTasksExecutor & get_instance(void);
	size_t pool_size(void);
	std::shared_ptr<Task> add_task(task_function_t task_function);
	void wait_for_all_done(void);

private:
	std::atomic<bool> terminate;
	std::atomic<int> tasks_in_progress;
	TSQueue<std::shared_ptr<Task>> tasks;
	std::vector<std::thread> pool;

	std::list<std::shared_ptr<Task>> tasks_with_precondition;
	
	DeferredTasksExecutor();
	~DeferredTasksExecutor();
	DeferredTasksExecutor(const DeferredTasksExecutor &) = delete;
	DeferredTasksExecutor(const DeferredTasksExecutor &&) = delete;
	DeferredTasksExecutor & operator=(const DeferredTasksExecutor &) = delete;
	DeferredTasksExecutor & operator=(const DeferredTasksExecutor &&) = delete;

	void worker_thread(size_t id);
	void precondition_checker_thread();
	bool next_task(std::shared_ptr<Task> &task);
	void terminate_and_join_all_threads(void);
};

#endif // _DEFERRED_TASKS_EXECUTOR_H
