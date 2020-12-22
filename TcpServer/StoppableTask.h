#ifndef STOPPABLE_TASK_H
#define STOPPABLE_TASK_H

#include <thread>
#include <future>

class StoppableTask {
public:
	virtual void run() = 0;

	void start()
	{
		if (_thread == NULL) {
			_thread = new std::thread(&StoppableTask::run, this);
		}
	}

protected:
	StoppableTask() :
		_futureObj(_exitSignal.get_future()),
		_thread(NULL)
	{
	}

	virtual ~StoppableTask()
	{
		_exitSignal.set_value();
		_thread->join();
		delete _thread;
	}

	void operator()()
	{
		run();
	}

	bool stopRequested()
	{
		if (_futureObj.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout){
			return false;
		}
		return true;
	}

private:
	StoppableTask(StoppableTask&& obj);

	std::promise<void> _exitSignal;
	std::future<void> _futureObj;
	std::thread* _thread;
};

#endif
