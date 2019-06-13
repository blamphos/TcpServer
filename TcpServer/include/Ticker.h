#ifndef TICKER_H
#define TICKER_H

#include "mbed.h"

class TickerWorker;

class Ticker {
public:
    Ticker();
    void attach(Callback<void()> cb, unsigned int t);
    void attach(void (*isr_func)(), unsigned int t);
    void attach_us(void (*isr_func)(), unsigned int t);

    template<typename T>
    void attach_us(T* tptr, void (T::*mptr)(), unsigned int t);
    void detach();

private:
    TickerWorker* _worker;
};

#endif
