#include "RawSerial.h"

RawSerial::RawSerial(PinName TxPin, PinName RxPin, int baudrate) {}

void RawSerial::attach(Callback<void()> cb, Serial::IrqTypeT iqrType) {
    _cb = cb;
}

void RawSerial::detach()
{
    _cb = NULL;
}
bool RawSerial::readable()
{
    return true;
}

void RawSerial::printf(const char*, ...)
{
}

void RawSerial::putc(char c)
{
}

char RawSerial::getc()
{
    return 0;
}
