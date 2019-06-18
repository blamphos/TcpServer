#include "Ticker.h"
#include "WorkerBase.h"

Ticker::Ticker()
{
    _worker = new WorkerBase(true);
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


