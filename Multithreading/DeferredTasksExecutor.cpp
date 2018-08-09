#include "stdafx.h"
#include <string>
#include "DeferredTasksExecutor.h"


DeferredTasksExecutor::DeferredTasksExecutor() : added(0), executed(0), terminate(false)
{
	size_t threads_count = std::thread::hardware_concurrency();
	if (threads_count == 0) {
		threads_count = 2;
	}

	pool.resize(threads_count);
	size_t worker_id = 0;
	try {
		for (auto &thr : pool) {
			thr = std::thread(&DeferredTasksExecutor::Worker, this, worker_id++);
		}
	} catch (...) {
		terminate = true;
		throw;
	}
}

DeferredTasksExecutor::~DeferredTasksExecutor()
{
	terminate = true;
	for (auto &thr : pool) {
		if (thr.joinable()) {
			thr.join();
		}
	}
}

void DeferredTasksExecutor::Worker(size_t worker_id)
{
	std::string id_str = std::to_string(worker_id);
	trace("Worker started");

	while (!terminate) {
		Task task;
		if (tasks.TryPop(task)) {
			trace(id_str + " => Execute task");
			task();
			{
				std::lock_guard<std::mutex> lock(mtx);
				++executed;
			}
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	trace("Worker stoped");
}
