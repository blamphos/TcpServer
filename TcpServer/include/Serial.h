#ifndef SERIAL_H
#define SERIAL_H

#include <stdarg.h>
#include <iostream>
#include "mbed.h"

class SerialDebug {
public:
    SerialDebug(PinName TxPin, PinName RxPin, int baudrate)
    {
    }

    void printf(const char* format, ...)
    {
        va_list args;
        va_start (args, format);
        vprintf (format, args);
        va_end (args);
    }

    void putc(char c)
    {
        std::cout << c;
    }

private:

};

#endif
