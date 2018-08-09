#include "stdafx.h"
#include "DeferredTasksExecutor.h"


void WorkerFunc(void)
{


}


DeferredTasksExecutor::DeferredTasksExecutor()
{
	size_t threads_count = std::thread::hardware_concurrency();
	if (threads_count == 0) {
		threads_count = 2;
	}

	pool.resize(threads_count);
	for (auto &thr : pool) {
		thr = std::thread(WorkerFunc);
	}
	
	int x = 0;
}


DeferredTasksExecutor::~DeferredTasksExecutor()
{
	for (auto &thr : pool) {
		if (thr.joinable()) {
			thr.join();
		}
	}
}
