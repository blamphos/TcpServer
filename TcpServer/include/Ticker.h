#ifndef TICKER_H
#define TICKER_H

#include <thread>
#include "mbed.h"

class Worker {
public:
    Worker() {

    }

    void doWork() {
        while(attached) {
            cb.call();
            wait_us(t);
        }
    }

public:
    unsigned int t;
    bool attached;

    Callback<void()> cb;
};

class Ticker {
public:
    Ticker() {

    }

    void attach_us(void (*isr_func)(), unsigned int t) {
        _worker.cb = callback(isr_func);
        _worker.attached = true;
        _worker.t = t;
        _thread = new std::thread(&Worker::doWork, &_worker);
    }

    template<typename T>
    void attach_us(T* tptr, void (T::*mptr)(), unsigned int t) {
        _worker.cb = callback(tptr, mptr);
        _worker.attached = true;
        _worker.t = t;
        _thread = new std::thread(&Worker::doWork, &_worker);
    }

    void detach() {
        _worker.attached = false;
    }

private:
    std::thread *_thread;
    Worker _worker;
};

#endif
