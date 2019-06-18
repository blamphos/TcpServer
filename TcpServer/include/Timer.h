#ifndef TIMER_H
#define TIMER_H

#include <chrono>

using namespace std::chrono;

class Timer {
public:
    Timer() {

    }

    void start() {
        _t1 = high_resolution_clock::now();
    }

    void stop() {
        _t2 = high_resolution_clock::now();
    };


    void reset() {

    }

    int read_us() {
        return duration_cast<microseconds>( _t2 - _t1 ).count();
    }

    int read_ms() {
        return duration_cast<milliseconds>( _t2 - _t1 ).count();
    }

private:
    high_resolution_clock::time_point _t1;
    high_resolution_clock::time_point _t2;
};

#endif
