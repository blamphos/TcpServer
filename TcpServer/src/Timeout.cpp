#include "Timeout.h"
#include "WorkerBase.h"

Timeout::Timeout()
{
    _worker = new WorkerBase(false);
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
