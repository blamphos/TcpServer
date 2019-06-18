#ifndef SERIAL_H
#define SERIAL_H

#include "mbed.h"
#include <cstdarg>

class Serial {
public:
    Serial(PinName TxPin, PinName RxPin, int baudrate);
    ~Serial();
    void printf(const char* format, ...);
    void print_args(const char* format, va_list args);
    void putc(char c);
    int waitKeyPress();
    static void str2buff(char* buff, const char* str);

private:

};

#endif
