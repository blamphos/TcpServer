#ifndef BUSOUT_H
#define BUSOUT_H

#include "mbed.h"
#include "DigitalOut.h"

class BusOut {
public:
    BusOut(PinName p0,
    PinName 	p1 = NC,
    PinName 	p2 = NC,
    PinName 	p3 = NC,
    PinName 	p4 = NC,
    PinName 	p5 = NC,
    PinName 	p6 = NC,
    PinName 	p7 = NC,
    PinName 	p8 = NC,
    PinName 	p9 = NC,
    PinName 	p10 = NC,
    PinName 	p11 = NC,
    PinName 	p12 = NC,
    PinName 	p13 = NC,
    PinName 	p14 = NC,
    PinName 	p15 = NC);

    BusOut(PinName pins[16]);

    int mask() {
        return _nc_mask;
    }

    int read();
    void write(int value);
    void operator= (int v) {
        write(v);
    }
    operator int() {
        return read();
    }

private:
    void connectPin(int i, PinName pin) {
        _pins[i] = new DigitalOut(pin);
        if (pin != NC) {
            _nc_mask |= (1 << i);
        }
    }

    DigitalOut *_pins[16];
    int _nc_mask;

    // Disallow
    //BusOut(const BusOut&);
    BusOut & operator = (const BusOut&);
};

#endif
