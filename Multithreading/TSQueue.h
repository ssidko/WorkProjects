#ifndef _TSQUEUE_H
#define _TSQUEUE_H

#include <queue>
#include <mutex>

template<typename T>
class TSQueue
{
public:
	TSQueue() {}
	explicit TSQueue(const TSQueue &) = default;
	explicit TSQueue(TSQueue &&) = default;
	TSQueue& operator =(const TSQueue &) = delete;
	
	void push(const T &element)
	{
		std::lock_guard<std::mutex> lock(mtx);
		queue.push(element);
	}

	bool try_pop(T &element)
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (!queue.empty()) {
			element = queue.front();
			queue.pop();
			return true;
		}		
		return false;
	}

	bool empty(void)
	{
		std::lock_guard<std::mutex> lock(mtx);
		return queue.empty();
	}

	size_t size(void)
	{
		std::lock_guard<std::mutex> lock(mtx);
		return queue.size();
	}
	
private:
	std::mutex mtx;
	std::queue<T> queue;
};

#endif // _TSQUEUE_H
