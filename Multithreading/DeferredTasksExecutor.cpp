#include <iostream>
#include <string>
#include <atomic>
#include <assert.h>
#include <algorithm>

#include "DeferredTasksExecutor.h"

namespace  {
	const size_t sleep_for_next_try_usec = 100;
	const size_t default_threads_count = 4;
};

Task::Task(task_function_t task_function, priority_t priority, precondition_t precond)
	: function(task_function), task_priority(priority), precondition(precond)
{
	this->set_status(TaskStatus::not_in_queue);
}

TaskStatus Task::status(void)
{
	return static_cast<TaskStatus>(static_cast<int>(task_status));
}

priority_t Task::priority(void)
{
	return task_priority;
}

void Task::wait_for_done()
{
	std::unique_lock<std::mutex> lock(cv_mtx);
	cv.wait(lock, [this]() {return this->status() == TaskStatus::done;});
}

void Task::wait_for_status_changed()
{
	std::unique_lock<std::mutex> lock(cv_mtx);
	TaskStatus prev_status = this->status();
	cv.wait(lock, [this, prev_status]() {return prev_status != this->status();});
}

bool Task::is_done(void)
{
	return status() == TaskStatus::done;
}

bool Task::ready_for_processing(void)
{
	return precondition();
}

void Task::operator ()(void)
{
	function();
}

void Task::set_status(TaskStatus new_status)
{
	task_status = static_cast<int>(new_status);
	cv.notify_all();
}

bool task_comp(const std::shared_ptr<Task> &lhs, const std::shared_ptr<Task> &rhs)
{
	return lhs->priority() > rhs->priority();
}

DeferredTasksExecutor & DeferredTasksExecutor::get_instance(void)
{
	static DeferredTasksExecutor instance;
	if (!instance.pool_started) {
		instance.start_worker_pool();
	}
	return instance;
}

void DeferredTasksExecutor::start_worker_pool(void)
{
	std::lock_guard<std::mutex> lock(pool_mtx);
	if (!pool_started) {
		size_t threads_count = std::thread::hardware_concurrency();
		if (threads_count == 0) {
			threads_count = default_threads_count;
		}
		pool.reserve(threads_count);
		try {
			for (size_t i = 0; i < threads_count; ++i) {
				pool.emplace_back(std::thread(&DeferredTasksExecutor::worker_thread, this));
			}
		}
		catch (...) {
			terminate_and_join_all_threads();
			throw;
		}
		pool_started = true;
	}
}

DeferredTasksExecutor::DeferredTasksExecutor() : terminate(false), pool_started(false), tasks_in_progress(0)
{
}

DeferredTasksExecutor::~DeferredTasksExecutor()
{
	terminate_and_join_all_threads();
}

size_t DeferredTasksExecutor::pool_size(void)
{
	return pool.size();
}

std::shared_ptr<Task> DeferredTasksExecutor::add_task(
	task_function_t task_function,
	priority_t priority,
	precondition_t precondition)
{
	auto task = std::make_shared<Task>(task_function, priority, precondition);
	add_task(task);
	return task;
}

void DeferredTasksExecutor::add_task(std::shared_ptr<Task> &task)
{
	std::lock_guard<std::mutex> lock(tasks_queue_mtx);
	tasks_queue.push_back(task);
	task->set_status(TaskStatus::in_queue);
	if (task->priority()) {
		std::sort(tasks_queue.begin(), tasks_queue.end(), task_comp);
	}
}

void DeferredTasksExecutor::cancel_task(std::shared_ptr<Task> &task)
{
	std::lock_guard<std::mutex> lock(tasks_queue_mtx);
	task->set_status(TaskStatus::cancel);
}

void DeferredTasksExecutor::wait_for_all_done(void)
{
	while (!tasks_queue.empty()) {
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_for_next_try_usec));
	}
	while (tasks_in_progress) {
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_for_next_try_usec));
	}
}

bool DeferredTasksExecutor::next_task(std::shared_ptr<Task> &task)
{
	std::lock_guard<std::mutex> lock(tasks_queue_mtx);
	if (!tasks_queue.empty()) {
		task = tasks_queue.front();

		++tasks_in_progress;
		assert(tasks_in_progress <= static_cast<int>(pool.size()));

		tasks_queue.pop_front();
		return true;
	}
	return false;
}

void DeferredTasksExecutor::worker_thread(void)
{
	while (!terminate) {
		std::shared_ptr<Task> task;
		if (next_task(task)) {

			if (task->status() == TaskStatus::in_queue) {
				if (task->ready_for_processing()) {
					task->set_status(TaskStatus::processing);
					(*task)();
					task->set_status(TaskStatus::done);
				} else {
					add_task(task);
				}				
			} else {
				assert(task->status() == TaskStatus::cancel);
				task->set_status(TaskStatus::canceled);			
			}

			--tasks_in_progress;
			assert(tasks_in_progress >= 0);


			//if (task->status() == TaskStatus::cancel) {
			//	task->set_status(TaskStatus::canceled);
			//	--tasks_in_progress;
			//	assert(tasks_in_progress >= 0);
			//	continue;
			//}

			//if (!task->ready_for_processing()) {
			//	add_task(task);
			//	--tasks_in_progress;
			//	assert(tasks_in_progress >= 0);
			//	continue;
			//}

			//task->set_status(TaskStatus::processing);
			//(*task)();
			//task->set_status(TaskStatus::done);

			//--tasks_in_progress;
			//assert(tasks_in_progress >= 0);
			
		} else {
			std::this_thread::sleep_for(std::chrono::microseconds(sleep_for_next_try_usec));
		}
	}
}

void DeferredTasksExecutor::terminate_and_join_all_threads(void)
{
	terminate = true;
	for (auto &thr : pool) {
		if (thr.joinable()) {
			thr.join();
		}
	}
	pool.resize(0);
}
