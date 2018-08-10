#ifndef _TSQUEUE_H
#define _TSQUEUE_H

#include <queue>
#include <mutex>

template<typename T>
class TSQueue
{
public:
	TSQueue() {}
	explicit TSQueue(const TSQueue &queue) = default;
	explicit TSQueue(TSQueue &&queue) = default;
	TSQueue& operator =(const TSQueue & queue) = delete;
	
	void push(const T &element)
	{
		std::lock_guard<std::mutex> lock(mtx);
		queue.push(element);
	}

	bool try_pop(T &element)
	{
		if (!queue.empty()) {
			std::lock_guard<std::mutex> lock(mtx);
			if (!queue.empty()) {
				element = queue.front();
				queue.pop();
				return true;
			}		
		}
		return false;
	}

	bool empty(void)
	{
		std::lock_guard<std::mutex> lock(mtx);
		return queue.empty();
	}
	
private:
	std::mutex mtx;
	std::queue<T> queue;
};

#endif // _TSQUEUE_H
