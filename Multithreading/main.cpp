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
		std::cout << "=> Generate " << tasks_count << " tasks and wait for all they done" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			added_tasks++;
			executor.add_task([&completed_tasks]() {
				std::cout << ".";
				std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100));
				completed_tasks++;
			});
		}

		executor.wait_for_all_done();

		std::cout << std::endl;
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
		std::cout << "=> Generate " << tasks_count << " tasks" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			added_tasks++;
			executor.add_task(test_task);
		}

		std::cout << "=> Adding a task that we will wait" << std::endl;
		added_tasks++;
		auto task = executor.add_task(test_task);

		assert((*task).status() == TaskStatus::in_queue);
		std::cout << "Task status: " << task_status_to_string((*task).status()) << std::endl;

		std::cout << "=> Generate " << tasks_count << " tasks." << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			added_tasks++;
			executor.add_task(test_task);
		}

		std::cout << "Waiting for task done" << std::endl;
		(*task).wait_for_done();

		assert((*task).status() == TaskStatus::done);
		std::cout << "Task status: " << task_status_to_string((*task).status()) << std::endl;

		std::cout << "Waiting for all done" << std::endl;
		executor.wait_for_all_done();

		std::cout << "# Task added: " << added_tasks << std::endl;
		std::cout << "# Task completed: " << completed_tasks << std::endl << std::endl;
	}


	{
		std::cout << std::endl;
		std::cout << "### Test tasks with precondition" << std::endl;

		auto task_1 = std::make_shared<Task>([]() {
			std::cout << "task_1 done" << std::endl;
		});

		std::cout << "=> Add task_2 with precondition: task_1.is_done() == true" << std::endl;
		auto task_2 = executor.add_task([]() {
			std::cout << "task_2 done" << std::endl;
		}, 0,
			[&task_1]() {return task_1->is_done();}
		);
		assert(task_2->status() == TaskStatus::wait_for_precondition);
		std::cout << "task_2 status: " << task_status_to_string(task_2->status()) << std::endl;


		std::cout << "=> Add task_3 with precondition: task_2.is_done() == true" << std::endl;
		auto task_3 = executor.add_task([]() {
			std::cout << "task_3 done" << std::endl;
		}, 0,
			[&task_2]() {return task_2->is_done(); }
		);
		assert(task_3->status() == TaskStatus::wait_for_precondition);
		std::cout << "task_3 status: " << task_status_to_string(task_3->status()) << std::endl;


		std::cout << "=> Generate " << tasks_count << " tasks" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			executor.add_task([]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100));
			});
		}
		
		std::cout << "=> Add task_1" << std::endl;
		executor.add_task(task_1);

		executor.wait_for_all_done();	
	}

	{	
		std::cout << std::endl;
		std::cout << "### Test tasks with priority" << std::endl;

		std::cout << "=> Generate " << tasks_count << " tasks" << std::endl;
		for (size_t i = 0; i < tasks_count; i++) {
			executor.add_task([]() {
				trace(".");
				std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 100));
			});
		}

		trace("\n=> Add task with higher priority\n");
		auto priority_task = executor.add_task([]() {
			trace("\nHigh priority task done\n");
		}, 10);

		executor.wait_for_all_done();
		std::cout << std::endl << "All done" << std::endl;
	}

	std::system("pause");
    return 0;
}

