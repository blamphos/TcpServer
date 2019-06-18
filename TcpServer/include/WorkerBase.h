#ifndef WORKER_BASE_H
#define WORKER_BASE_H

#include <thread>
#include "mbed.h"

class WorkerBase {
public:
    WorkerBase(bool do_loop);
    void run();
    void threadFunc();

	Callback<void()> cb;

    unsigned int t;
    bool attached;

private:
    std::thread* _thread;
    bool _do_loop;
};

#endif
