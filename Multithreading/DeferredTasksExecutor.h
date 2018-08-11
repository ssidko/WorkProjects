#ifndef _DEFERRED_TASKS_EXECUTOR_H
#define _DEFERRED_TASKS_EXECUTOR_H

#include <vector>
#include <list>
#include <memory>
#include <thread>
#include <atomic>

#include "DeferredTasksExecutor.h"
#include "TSQueue.h"

using task_function_t = std::function<void()>;
using precondition_t = std::function<bool()>;

enum class TaskStatus {
	not_in_queue,
	wait_for_precondition,
	in_queue,
	processing,
	done
};

void trace(const std::string str);
const char *task_status_to_string(TaskStatus status);

class DeferredTasksExecutor;

class Task
{
	friend DeferredTasksExecutor;
public:
	Task(task_function_t task_function, precondition_t precond = []() {return true;});

	Task(const Task &) = delete;
	Task(const Task &&) = delete;
	Task & operator =(const Task &) = delete;
	Task & operator =(const Task &&) = delete;

	TaskStatus status(void);	
	bool is_done(void);
	void wait_for_done();
	void operator ()(void);

private:
	task_function_t function;
	precondition_t precondition;
	std::atomic<int> task_status;

	void set_status(TaskStatus new_status);
	bool ready_for_processing();
};

class DeferredTasksExecutor
{	
public:
	static DeferredTasksExecutor & get_instance(void);
	size_t pool_size(void);
	std::shared_ptr<Task> add_task(task_function_t task_function);
	std::shared_ptr<Task> add_task(task_function_t task_function, precondition_t precondition);
	void add_task(std::shared_ptr<Task> &task);
	void wait_for_all_done(void);

private:
	std::atomic<bool> terminate;
	std::atomic<int> tasks_in_progress;
	TSQueue<std::shared_ptr<Task>> tasks;
	std::vector<std::thread> pool;

	std::mutex deferred_tasks_mtx;
	std::list<std::shared_ptr<Task>> deferred_tasks;
	
	DeferredTasksExecutor();
	~DeferredTasksExecutor();
	DeferredTasksExecutor(const DeferredTasksExecutor &) = delete;
	DeferredTasksExecutor(const DeferredTasksExecutor &&) = delete;
	DeferredTasksExecutor & operator=(const DeferredTasksExecutor &) = delete;
	DeferredTasksExecutor & operator=(const DeferredTasksExecutor &&) = delete;

	void worker_thread(void);
	void precondition_checker_thread(void);

	void add_task_to_deferred_tasks(std::shared_ptr<Task> task);
	bool next_task(std::shared_ptr<Task> &task);
	void terminate_and_join_all_threads(void);
};

#endif // _DEFERRED_TASKS_EXECUTOR_H
