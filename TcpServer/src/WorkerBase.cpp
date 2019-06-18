#include "WorkerBase.h"

WorkerBase::WorkerBase(bool do_loop) :
    t(0),
    attached(false),
    _thread(NULL),
    _do_loop(do_loop)
{

}

void WorkerBase::run()
{
    _thread = new std::thread(&WorkerBase::threadFunc, this);
}

void WorkerBase::threadFunc()
{
    if (_do_loop) {
        while (1) {
            wait_us(t);
            if (!attached) {
                break;
            }
            cb.call();
        }
    } else {
        wait_us(t);
        if (attached) {
            attached = false;
            cb.call();
        }
    }
}
