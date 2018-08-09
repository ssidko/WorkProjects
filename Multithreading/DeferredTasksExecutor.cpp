#include "stdafx.h"
#include <string>
#include "DeferredTasksExecutor.h"


void WorkerFunc(DeferredTasksExecutor &executor, size_t id)
{
	std::string worker_id = std::to_string(id);
	trace("Worker started");

	while (!executor.terminate) {
		Task task;
		if (executor.tasks.Pop(task)) {
			trace(worker_id + " => Execute task");
			task();
			{
				std::lock_guard<std::mutex> lock(executor.mtx);
				++executor.executed;
			}
		} else {			
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	trace("Worker stoped");
}

DeferredTasksExecutor::DeferredTasksExecutor() : added(0), executed(0), terminate(false)
{
	size_t threads_count = std::thread::hardware_concurrency();
	if (threads_count == 0) {
		threads_count = 2;
	}

	pool.resize(threads_count);
	size_t worker_id = 0;
	for (auto &thr : pool) {
		thr = std::thread(&WorkerFunc, std::ref(*this), worker_id++);
	}
	
	int x = 0;
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
