#include <iostream>
#include <string>
#include <atomic>
#include <assert.h>
#include <algorithm>

#include "DeferredTasksExecutor.h"

enum Constants {
	sleep_for_next_try_usec = 100,
	default_threads_count = 4,
};

void trace(const std::string str)
{
	static std::mutex trace_mtx;
	std::lock_guard<std::mutex> lock(trace_mtx);
	std::cout << str ;
}

const char *task_status_to_string(TaskStatus status)
{
	switch (status) {
	case TaskStatus::not_in_queue: return "not_in_queue";
	case TaskStatus::wait_for_precondition: return "wait_for_precondition";
	case TaskStatus::in_queue: return "in_queue";
	case TaskStatus::processing: return "processing";
	case TaskStatus::done: return "done";
	default: return "unknown";
	}
}

Task::Task(task_function_t task_function, priority_t priority, precondition_t precond)
	: function(task_function), task_priority(priority), precondition(precond)
{
	set_status(TaskStatus::not_in_queue);
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
	while (status() != TaskStatus::done) {
		std::this_thread::sleep_for(std::chrono::microseconds(Constants::sleep_for_next_try_usec));
	}
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
}

bool task_comp(const std::shared_ptr<Task> &lhs, const std::shared_ptr<Task> &rhs)
{
	return lhs->priority() > rhs->priority();
}

DeferredTasksExecutor & DeferredTasksExecutor::get_instance(void)
{
	static DeferredTasksExecutor instance;
	return instance;
}

DeferredTasksExecutor::DeferredTasksExecutor() : terminate(false)
{
	size_t threads_count = std::thread::hardware_concurrency();
	if (threads_count == 0) {
		threads_count = Constants::default_threads_count;
	}

	pool.resize(threads_count + 1);
	try {
		auto b = pool.begin();
		(*b++) = std::thread(&DeferredTasksExecutor::precondition_checker_thread, this);
		for (auto e = pool.end(); b != e; b++) {
			(*b) = std::thread(&DeferredTasksExecutor::worker_thread, this);
		}

	} catch (...) {
		terminate_and_join_all_threads();
		throw;
	}
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
	if (task->ready_for_processing()) {
		std::lock_guard<std::mutex> lock(tasks_queue_mtx);
		tasks_queue.push_back(task);
		task->set_status(TaskStatus::in_queue);
		std::sort(tasks_queue.begin(), tasks_queue.end(), task_comp);
	} else {
		add_task_to_deferred_tasks(task);
		task->set_status(TaskStatus::wait_for_precondition);
	}
}

//void DeferredTasksExecutor::cancel_task(std::shared_ptr<Task> &task)
//{
//
//	if (task->status() == TaskStatus::wait_for_precondition) {
//		std::lock_guard<std::mutex> lock(deferred_tasks_mtx);
//		if (task->status() == TaskStatus::wait_for_precondition) {
//
//			auto it = std::remove_if(deferred_tasks.begin(), deferred_tasks.end(), [&task](std::shared_ptr<Task> &t) {
//				return t.get() == task.get();
//			});
//		
//		}
//	} 
//	if (task->status() == TaskStatus::in_queue) {
//	
//	}
//}

void DeferredTasksExecutor::wait_for_all_done(void)
{
	while (!tasks_queue.empty()) {
		std::this_thread::sleep_for(std::chrono::microseconds(Constants::sleep_for_next_try_usec));
	}
	while (tasks_in_progress) {
		std::this_thread::sleep_for(std::chrono::microseconds(Constants::sleep_for_next_try_usec));
	}
}

void DeferredTasksExecutor::add_task_to_deferred_tasks(std::shared_ptr<Task> task)
{
	std::lock_guard<std::mutex> lock(deferred_tasks_mtx);
	deferred_tasks.push_back(task);
}

bool DeferredTasksExecutor::next_task(std::shared_ptr<Task> &task)
{
	std::lock_guard<std::mutex> lock(tasks_queue_mtx);
	if (!tasks_queue.empty()) {

		task = tasks_queue.front();
		tasks_queue.pop_front();

		tasks_in_progress++;
		assert(tasks_in_progress <= static_cast<int>(pool.size()));

		return true;
	}
	return false;
}

void DeferredTasksExecutor::worker_thread(void)
{
	while (!terminate) {
		std::shared_ptr<Task> task;
		if (next_task(task)) {

			(*task).set_status(TaskStatus::processing);
			(*task)();
			(*task).set_status(TaskStatus::done);
			tasks_in_progress--;

			assert(tasks_in_progress >= 0);

		} else {
			std::this_thread::sleep_for(std::chrono::microseconds(Constants::sleep_for_next_try_usec));
		}
	}
}

void DeferredTasksExecutor::precondition_checker_thread(void)
{
	while (!terminate) {		
		if (!deferred_tasks.empty()) {
			
			std::lock_guard<std::mutex> lock(deferred_tasks_mtx);

			auto it = deferred_tasks.begin();
			while (it != deferred_tasks.end()) {
				auto task = (*it);
				if (task->ready_for_processing()) {
					it = deferred_tasks.erase(it);
					add_task(task);
					task->set_status(TaskStatus::in_queue);
				} else {
					++it;
				}			
			}
		}
		std::this_thread::sleep_for(std::chrono::microseconds(Constants::sleep_for_next_try_usec));
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
}
