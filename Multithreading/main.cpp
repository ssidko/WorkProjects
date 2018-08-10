#include "stdafx.h"

#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <atomic>

#include "DeferredTasksExecutor.h"
#include "TSQueue.h"
#include "trace.h"

int main()
{
	std::srand(static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count()));

	size_t added_tasks = 0;
	std::atomic<size_t> completed_tasks = 0;

	size_t tasks_count = 10000;
	{

		DeferredTasksExecutor executor;
		for (size_t i = 0; i < tasks_count; i++) {
			trace("Add task");
			added_tasks++;
			executor.add_task([&completed_tasks]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100));
				completed_tasks++;
			});
		}
		executor.wait_for_complete_all_tasks();

		std::cout << "----------------- " << std::endl;
		std::cout << "# Task added: " << added_tasks << std::endl;
		std::cout << "# Task completed: " << completed_tasks << std::endl;

	}

	assert(added_tasks == completed_tasks);

	std::system("pause");
    return 0;
}

