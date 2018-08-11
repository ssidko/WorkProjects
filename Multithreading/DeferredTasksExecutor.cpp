#include "stdafx.h"

#include <iostream>
#include <string>
#include <atomic>
#include <assert.h>

#include "DeferredTasksExecutor.h"


void trace(const std::string str)
{
	static std::mutex trace_mtx;
	std::lock_guard<std::mutex> lock(trace_mtx);
	std::cout << str << std::endl;
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

DeferredTask::DeferredTask(TaskFunction task_function)
	: function(task_function),
	task_status(TaskStatus::not_in_queue)
{
}

TaskStatus DeferredTask::status(void)
{
	std::lock_guard<std::mutex> lock(mtx);
	return task_status;
}

void DeferredTask::wait_for_done()
{
	while (status() != TaskStatus::done) {
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_for_next_try_usec));
	}
}

bool DeferredTask::is_done(void)
{
	return status() == TaskStatus::done;
}

void DeferredTask::operator ()(void)
{
	set_status(TaskStatus::processing);
	function();
	set_status(TaskStatus::done);
}

void DeferredTask::set_status(TaskStatus new_status)
{
	std::lock_guard<std::mutex> lock(mtx);
	task_status = new_status;
}

DeferredTasksExecutor & DeferredTasksExecutor::get_instance(void)
{
	static DeferredTasksExecutor instance;
	return instance;
}

DeferredTasksExecutor::DeferredTasksExecutor() : terminate(false)
{
	const size_t default_threads_count = 4;
	size_t threads_count = std::thread::hardware_concurrency();
	if (threads_count == 0) {
		threads_count = default_threads_count;
	}

	pool.resize(threads_count);
	size_t worker_id = 0;
	try {
		for (auto &worker : pool) {
			worker = std::thread(&DeferredTasksExecutor::worker_thread, this, worker_id++);
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

std::shared_ptr<DeferredTask> DeferredTasksExecutor::add_task(TaskFunction task_function)
{
	auto task = std::make_shared<DeferredTask>(task_function);
	tasks.push(task);
	task.get()->set_status(TaskStatus::in_queue);
	return task;
}

void DeferredTasksExecutor::wait_for_all_done(void)
{
	while (!tasks.empty()) {
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_for_next_try_usec));
	}
	while (tasks_in_progress) {
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_for_next_try_usec));
	}
}

bool DeferredTasksExecutor::next_task(std::shared_ptr<DeferredTask> &task)
{
	if (tasks.try_pop(task)){
		tasks_in_progress++;

		assert(tasks_in_progress <= static_cast<int>(pool.size()));

		return true;
	}
	return false;
}

void DeferredTasksExecutor::worker_thread(size_t worker_id)
{
	while (!terminate) {
		std::shared_ptr<DeferredTask> task;
		if (next_task(task)) {

			(*task).set_status(TaskStatus::processing);
			(*task)();
			(*task).set_status(TaskStatus::done);
			tasks_in_progress--;

			assert(tasks_in_progress >= 0);

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
}
