#ifndef DIGITAL_IN
#define DIGITAL_IN

#include "mbed.h"

class DigitalIn {
public:
    DigitalIn(PinName pin) {}
    int read() { return 0; }
    void mode(PullModeT type) {}
};

#endif
