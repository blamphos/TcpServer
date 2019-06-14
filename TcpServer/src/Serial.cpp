#include <iostream>
#include <cstdarg>
#include "conio.h"
#include "Serial.h"

Serial::Serial(PinName TxPin, PinName RxPin, int baudrate)
{

}

void Serial::printf(const char* format, ...)
{
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
}

void Serial::putc(char c)
{
    //std::cout << c;
}

int Serial::waitKeyPress()
{
    return _kbhit();
}
