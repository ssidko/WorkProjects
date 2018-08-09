#include "stdafx.h"
#include <cstdlib>
#include "DeferredTasksExecutor.h"
#include "TSQueue.h"
#include "trace.h"

void task_func(void)
{
	auto duration = std::chrono::milliseconds(std::rand() % 1000);
	std::this_thread::sleep_for(duration);
}

int main()
{
	std::srand(static_cast<int>(std::chrono::system_clock::now().time_since_epoch().count()));

	{
		DeferredTasksExecutor task_executor;
		for (int i = 0; i < 100; i++) {
			trace("Add task");
			task_executor.AddTask(&task_func);
		}
		for (int i = 0; i < 100; i++) {
			trace("Add task");
			task_executor.AddTask([]() {
				std::this_thread::sleep_for(std::chrono::milliseconds(std::rand() % 1000));
			});
		}
		std::this_thread::sleep_for(std::chrono::seconds(20));
		int x = 0;
	}

	std::system("pause");

    return 0;
}

