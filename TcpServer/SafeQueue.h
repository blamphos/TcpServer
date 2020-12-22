#ifndef SAFEQUEUE_H
#define SAFEQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template <class T>
class SafeQueue {
public:
	SafeQueue(void) : 
		_queue(),
		_mutex(),
		_cond()
	{
	
	}

	~SafeQueue(void)
	{
	
	}

	void enqueue(T t)
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_queue.push(t);
		_cond.notify_one();
	}

	T dequeue(void)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		while (_queue.empty())
		{
			// Release lock as long as the wait and require it afterwards.
			_cond.wait(lock);
		}
		T val = _queue.front();
		_queue.pop();
		return val;
	}

	bool dequeue(T& value, int timeout_ms)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (_queue.empty() && _cond.wait_for(lock, std::chrono::milliseconds(timeout_ms)) == std::cv_status::timeout) {
			if (_queue.empty()) {
				return false;
			}
		}

		value = _queue.front();
		_queue.pop();
		return true;
	}

private:
	std::queue<T> _queue;
	mutable std::mutex _mutex;
	std::condition_variable _cond;
};

#endif
