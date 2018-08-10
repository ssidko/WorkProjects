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

DeferredTasksExecutor::DeferredTasksExecutor(size_t threads_count) : terminate(false)
{
	if (threads_count == 0) {
		threads_count = std::thread::hardware_concurrency();
		if (threads_count == 0) {
			threads_count = 2;
		}
	}

	pool.resize(threads_count);
	size_t worker_id = 0;
	try {
		for (auto &worker : pool) {
			worker = std::thread(&DeferredTasksExecutor::worker_func, this, worker_id++);
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

void DeferredTasksExecutor::add_task(TaskFunction task)
{
	tasks.push(task);
}

void DeferredTasksExecutor::wait_for_complete_all_tasks(void)
{
	while (!tasks.empty()) {
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_for_next_try_usec));
	}
	while (tasks_in_progress) {
		std::this_thread::sleep_for(std::chrono::microseconds(sleep_for_next_try_usec));
	}
}

bool DeferredTasksExecutor::next_task(TaskFunction &task)
{
	if (tasks.try_pop(task)){
		tasks_in_progress++;

		assert(tasks_in_progress <= static_cast<int>(pool.size()));

		return true;
	}
	return false;
}

void DeferredTasksExecutor::worker_func(size_t worker_id)
{
	std::string worker_name("Worker ");
	worker_name += std::to_string(worker_id);

	trace(worker_name + " started");

	while (!terminate) {
		TaskFunction task;
		if (next_task(task)) {
			trace(worker_name + " => Execute task");

			task();
			tasks_in_progress--;

			assert(tasks_in_progress >= 0);

		} else {
			std::this_thread::sleep_for(std::chrono::microseconds(sleep_for_next_try_usec));
		}
	}

	trace(worker_name + " stoped");
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
