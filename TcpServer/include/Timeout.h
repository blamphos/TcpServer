#ifndef TIMEOUT_H
#define TIMEOUT_H

#include <thread>
#include <iostream>
#include "mbed.h"

class TimeoutWorker {
public:
    TimeoutWorker() {

    }

    void doWork() {
        //while(attached) {
            wait_us(t);
            cb.call();
        //}
    }

public:
    unsigned int t;
    bool attached;

    Callback<void()> cb;
};

class Timeout {
public:
    Timeout() {

    }

    void attach(Callback<void()> cb, unsigned int t) {
        _worker.cb = cb;
        _worker.attached = true;
        _worker.t = t;
        _thread = new std::thread(&TimeoutWorker::doWork, &_worker);
    }

    void attach_us(void (*isr_func)(), unsigned int t) {
        _worker.cb = callback(isr_func);
        _worker.attached = true;
        _worker.t = t;
        _thread = new std::thread(&TimeoutWorker::doWork, &_worker);
    }

    template<typename T>
    void attach_us(T* tptr, void (T::*mptr)(), unsigned int t) {
        _worker.cb = callback(tptr, mptr);
        _worker.attached = true;
        _worker.t = t;
        _thread = new std::thread(&TimeoutWorker::doWork, &_worker);
    }

    void detach() {
        _worker.attached = false;
    }

private:
    std::thread *_thread;
    TimeoutWorker _worker;
};

#endif
