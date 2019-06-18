#include <thread>
#include "Ticker.h"

class TickerWorker {
public:
    TickerWorker() : t(0), attached(false), _thread(NULL)
    {

    }

    void run()
    {
        _thread = new std::thread(&TickerWorker::threadFunc, this);
    }

    void threadFunc() {
        while(attached) {
            wait_us(t);
            cb.call();
        }
    }

	Callback<void()> cb;

    unsigned int t;
    bool attached;

private:
    std::thread* _thread;
};

Ticker::Ticker()
{
    _worker = new TickerWorker();
}

void Ticker::attach(Callback<void()> cb, unsigned int t) {
    _worker->cb = cb;
    _worker->attached = true;
    _worker->t = t * 1e6;
    _worker->run();
}

void Ticker::attach(void (*isr_func)(), unsigned int t) {
    _worker->cb = callback(isr_func);
    _worker->attached = true;
    _worker->t = t * 1e6;
    _worker->run();
}

void Ticker::attach_us(void (*isr_func)(), unsigned int t) {
    _worker->cb = callback(isr_func);
    _worker->attached = true;
    _worker->t = t;
    _worker->run();
}

template<typename T>
void Ticker::attach_us(T* tptr, void (T::*mptr)(), unsigned int t) {
    _worker->cb = callback(tptr, mptr);
    _worker->attached = true;
    _worker->t = t;
    _worker->run();
}

void Ticker::detach() {
    _worker->attached = false;
}
