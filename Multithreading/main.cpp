#include "stdafx.h"

#include <cstdlib>
#include <iostream>
#include <assert.h>
#include <atomic>

#include "DeferredTasksExecutor.h"


int main()
{
	std::srand(static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count()));

	DeferredTasksExecutor &executor = DeferredTasksExecutor::get_instance();
	std::cout << "Pool size: " << executor.pool_size() << std::endl;	

	size_t added_tasks = 0;
	std::atomic<size_t> completed_tasks = 0;

	size_t tasks_count = 1000;

	{
		std::cout << "Generate " << tasks_count << " tasks and wait for all they done" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			added_tasks++;
			executor.add_task([&completed_tasks]() {
				std::cout << ".";
				std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100));
				completed_tasks++;
			});
		}

		executor.wait_for_all_done();

		std::cout << "\n#--------------------" << std::endl;
		std::cout << "# Task added: " << added_tasks << std::endl;
		std::cout << "# Task completed: " << completed_tasks << std::endl << std::endl;
	}

	{
		added_tasks = 0;
		completed_tasks = 0;

		auto test_task = [&completed_tasks]() {
			std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100));
			completed_tasks++;
		};

		std::cout << std::endl;
		std::cout << "Generate " << tasks_count << " tasks" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			added_tasks++;
			executor.add_task(test_task);
		}

		std::cout << "Adding a task that we will wait" << std::endl;
		added_tasks++;
		auto task = executor.add_task(test_task);

		assert((*task).status() == TaskStatus::in_queue);
		std::cout << "Task status: " << task_status_to_string((*task).status()) << std::endl;

		std::cout << "Generate addition " << tasks_count << " tasks." << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			added_tasks++;
			executor.add_task([&completed_tasks]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100));
				completed_tasks++;
			});
		}

		std::cout << "Waiting for task done" << std::endl;
		(*task).wait_for_done();

		assert((*task).status() == TaskStatus::done);
		std::cout << "Task status: " << task_status_to_string((*task).status()) << std::endl;

		std::cout << "Waiting for all done" << std::endl;
		executor.wait_for_all_done();

		std::cout << "#--------------------" << std::endl;
		std::cout << "# Task added: " << added_tasks << std::endl;
		std::cout << "# Task completed: " << completed_tasks << std::endl << std::endl;
	}








	std::system("pause");
    return 0;
}

