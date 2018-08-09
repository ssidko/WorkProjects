
#include "stdafx.h"
#include "DeferredTasksExecutor.h"
#include "TSQueue.h"

void task_func(void)
{
	auto duration = std::chrono::microseconds(std::rand());
	std::this_thread::sleep_for(duration);
}

int main()
{
	DeferredTasksExecutor task_executor;
	task_executor.AddTask(&task_func);
    return 0;
}

