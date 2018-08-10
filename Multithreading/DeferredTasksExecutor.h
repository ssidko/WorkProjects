#ifndef _DEFERRED_TASKS_EXECUTOR_H
#define _DEFERRED_TASKS_EXECUTOR_H

#include <vector>
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
	wait_for_precondition,
	in_queue,
	processing,
	done
};

class DeferredTask
{
public:
	DeferredTask(TaskFunction function_, std::function<bool()> precondition_ = []() {return true; }) 
		: function(function_), task_status(TaskStatus::in_queue), precondition(precondition_)
	{
	}

	TaskStatus status(void)
	{
		std::lock_guard<std::mutex> lock(mtx);
		return task_status;
	}
	
	void wait()
	{
		while(status() != TaskStatus::done) {
			std::this_thread::sleep_for(std::chrono::microseconds(sleep_for_next_try_usec));
		}
	}

	bool is_done(void)
	{
		return status() == TaskStatus::done;
	}

	void operator ()(void) {
		set_status(TaskStatus::processing);
		function();
		set_status(TaskStatus::done);
	}
private:
	TaskFunction function;
	std::function<bool()> precondition;
	std::mutex mtx;
	TaskStatus task_status;

	void set_status(TaskStatus new_status) {
		std::lock_guard<std::mutex> lock(mtx);
		task_status = new_status;
	}
};

class DeferredTasksExecutor
{	
public:
	DeferredTasksExecutor(size_t threads_count = 0);
	~DeferredTasksExecutor();

	void add_task(TaskFunction task);
	void wait_for_complete_all_tasks(void);

	// void cancel_task(void);

private:
	std::atomic<bool> terminate;
	std::atomic<int> tasks_in_progress;

	TSQueue<TaskFunction> tasks;
	std::vector<std::thread> pool;


	void worker_func(size_t id);
	bool next_task(TaskFunction &task);
	void terminate_and_join_all_threads(void);
};

#endif // _DEFERRED_TASKS_EXECUTOR_H
