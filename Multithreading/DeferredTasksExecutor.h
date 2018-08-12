#ifndef _DEFERRED_TASKS_EXECUTOR_H
#define _DEFERRED_TASKS_EXECUTOR_H

#include <vector>
#include <list>
#include <deque>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

using task_function_t = std::function<void()>;
using precondition_t = std::function<bool()>;
using priority_t = unsigned int;

enum class TaskStatus {
	not_in_queue,
	in_queue,
	processing,
	cancel,
	canceled,
	done
};

class DeferredTasksExecutor;

class Task
{
	friend DeferredTasksExecutor;
public:
	Task(task_function_t task_function, priority_t priority = 0, precondition_t precond = []() {return true;});

	Task(const Task &) = delete;
	Task(const Task &&) = delete;
	Task & operator =(const Task &) = delete;
	Task & operator =(const Task &&) = delete;

	TaskStatus status(void);
	priority_t priority(void);
	bool is_done(void);
	void wait_for_done();
	void wait_for_status_changed();
	void operator ()(void);

private:
	task_function_t function;
	const priority_t task_priority;
	precondition_t precondition;
	std::atomic<int> task_status;
	std::mutex cv_mtx;
	std::condition_variable cv;

	void set_status(TaskStatus new_status);
	bool ready_for_processing();
};

class DeferredTasksExecutor
{	
public:
	static DeferredTasksExecutor & get_instance(void);
	size_t pool_size(void);
	std::shared_ptr<Task> add_task(task_function_t task_function, priority_t priority = 0, precondition_t precondition = []() {return true;});
	void add_task(std::shared_ptr<Task> &task);	
	void cancel_task(std::shared_ptr<Task> &task);	
	void wait_for_all_done(void);

private:
	std::atomic<bool> terminate;
	std::atomic<int> tasks_in_progress;
	std::atomic<bool> pool_started;
	std::vector<std::thread> pool;

	std::mutex tasks_queue_mtx;
	std::deque<std::shared_ptr<Task>> tasks_queue;

	DeferredTasksExecutor();
	~DeferredTasksExecutor();
	DeferredTasksExecutor(const DeferredTasksExecutor &) = delete;
	DeferredTasksExecutor(const DeferredTasksExecutor &&) = delete;
	DeferredTasksExecutor & operator=(const DeferredTasksExecutor &) = delete;
	DeferredTasksExecutor & operator=(const DeferredTasksExecutor &&) = delete;

	void worker_thread(void);
	bool next_task(std::shared_ptr<Task> &task);
	void start_worker_pool(void);
	void terminate_and_join_all_threads(void);
};

#endif // _DEFERRED_TASKS_EXECUTOR_H
