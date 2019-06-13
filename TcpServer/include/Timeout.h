#ifndef TIMEOUT_H
#define TIMEOUT_H

#include "mbed.h"
#include "Callback.h"

class TimeoutWorker;

class Timeout {
public:
    Timeout();
    void attach(Callback<void()> cb, unsigned int t);
    void attach(void (*isr_func)(), unsigned int t);
    void attach_us(void (*isr_func)(), unsigned int t);

    template<typename T>
    void attach_us(T* tptr, void (T::*mptr)(), unsigned int t);
    void detach();

private:
    TimeoutWorker* _worker;
};

#endif
