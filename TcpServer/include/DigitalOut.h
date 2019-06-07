#ifndef DIGITALOUT
#define DIGITALOUT

#include "mbed.h"

class DigitalOut {
public:
    DigitalOut(PinName pin, int init_state = 0) {}
    void write(int value) {}
};

#endif
