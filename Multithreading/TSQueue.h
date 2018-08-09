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
	
	void Push(const T &element)
	{
		std::lock_guard<std::mutex> lock(mtx);
		queue.push(element);
		cv.notify_one();
	}

	void WaitAndPop(T &element)
	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this]() {return !queue.empty(); });
		element = queue.front();
		queue.pop();		
	}
	
private:
	std::mutex mtx;
	std::condition_variable cv;
	std::queue<T> queue;
};

#endif // _TSQUEUE_H
