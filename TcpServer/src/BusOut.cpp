#include "BusOut.h"

BusOut::BusOut(PinName p0,
    PinName 	p1,
    PinName 	p2,
    PinName 	p3,
    PinName 	p4,
    PinName 	p5,
    PinName 	p6,
    PinName 	p7,
    PinName 	p8,
    PinName 	p9,
    PinName 	p10,
    PinName 	p11,
    PinName 	p12,
    PinName 	p13,
    PinName 	p14,
    PinName 	p15) : _nc_mask(0)
    {
        connectPin(0, p0);
        connectPin(1, p1);
        connectPin(2, p2);
        connectPin(3, p3);
        connectPin(4, p4);
        connectPin(5, p5);
        connectPin(6, p6);
        connectPin(7, p7);
        connectPin(8, p8);
        connectPin(9, p9);
        connectPin(10, p10);
        connectPin(11, p11);
        connectPin(12, p12);
        connectPin(13, p13);
        connectPin(14, p14);
        connectPin(15, p15);

        write(0);
    }

BusOut::BusOut(PinName pins[16])
{
    for(int i = 0; i < 16; i++) {
        _pins[i] = new DigitalOut(pins[i]);
    }
}

int BusOut::read()
{
    int value = 0;

    for(int i = 0; i < 16; i++) {
        int bitmask = (1 << i);
        if ((_nc_mask & bitmask) != 0) {
            if (_pins[i]->read() != 0) {
                value |= bitmask;
            }
        }
    }
    return value;
}

void BusOut::write(int value)
{
    for(int i = 0; i < 16; i++) {
        int bitmask = (1 << i);
        if ((value & _nc_mask & bitmask) != 0) {
            (*_pins[i]) = 1;
        }
        else {
            (*_pins[i]) = 0;
        }
    }
}
