#ifndef SERIAL_H
#define SERIAL_H

#include "mbed.h"

class Serial {
public:
    Serial(PinName TxPin, PinName RxPin, int baudrate);
    ~Serial();
    void printf(const char* format, ...);
    void putc(char c);
    void readFromFile(const char* filename, char* buff, int len);
    int waitKeyPress();

private:

};

#endif
