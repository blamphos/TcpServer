#ifndef SERIAL_H
#define SERIAL_H

#include "mbed.h"

    enum IrqTypeTT {
        RxIrqT,
        TxIrqT,
    };

class Serial {
public:


    Serial(PinName TxPin, PinName RxPin, int baudrate);
    void printf(const char* format, ...);
    void putc(char c);
    int waitKeyPress();

private:

};

#endif
