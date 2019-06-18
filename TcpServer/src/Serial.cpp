#include <iostream>
#include <cstdarg>
#include "conio.h"
#include "Serial.h"

Serial::Serial(PinName TxPin, PinName RxPin, int baudrate)
{

}

Serial::~Serial()
{
    std::cin.get();
}

void Serial::printf(const char* format, ...)
{
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
}

void Serial::print_args(const char* format, va_list args)
{
    vprintf (format, args);
}

void Serial::putc(char c)
{
    std::cout << c;
}

void Serial::str2buff(char* buff, const char* str)
{
    sprintf(buff, str);
}

int Serial::waitKeyPress()
{
    return _kbhit();
}
