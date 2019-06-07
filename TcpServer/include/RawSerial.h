#ifndef RAW_SERIAL
#define RAW_SERIAL

#include "mbed.h"

class RawSerial {
public:
    RawSerial(PinName TxPin, PinName RxPin, int baudrate) {}

    bool readable() { return true; }
    void printf(const char*, ...) {}
    void putc(char c) {}
    char getc()
    {
        return 0;
    }
};

#endif
