#include <thread>
#include "Timeout.h"

class TimeoutWorker {
public:
	TimeoutWorker() : t(0), attached(false), _thread(NULL)
    {

    }

    void run()
    {
        _thread = new std::thread(&TimeoutWorker::threadFunc, this);
    }

    void threadFunc() {
        //while(attached) {
            wait_us(t);
            cb.call();
        //}
    }

	Callback<void()> cb;
	unsigned int t;
    bool attached;

private:
    std::thread* _thread;
};

Timeout::Timeout()
{
    _worker = new TimeoutWorker();
}

void Timeout::attach(Callback<void()> cb, unsigned int t) {
    _worker->cb = cb;
    _worker->attached = true;
    _worker->t = t * 1e6;
    _worker->run();
}

void Timeout::attach(void (*isr_func)(), unsigned int t) {
    _worker->cb = callback(isr_func);
    _worker->attached = true;
    _worker->t = t * 1e6;
    _worker->run();
}

void Timeout::attach_us(void (*isr_func)(), unsigned int t) {
    _worker->cb = callback(isr_func);
    _worker->attached = true;
    _worker->t = t;
    _worker->run();
}

template<typename T>
void Timeout::attach_us(T* tptr, void (T::*mptr)(), unsigned int t) {
    _worker->cb = callback(tptr, mptr);
    _worker->attached = true;
    _worker->t = t;
    _worker->run();
}

void Timeout::detach() {
    _worker->attached = false;
}
