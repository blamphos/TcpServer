#ifndef RAW_SERIAL
#define RAW_SERIAL

#include "mbed.h"

namespace Serial {
    enum IrqTypeT {
        RxIrq,
        TxIrq,
    };
}

class RawSerial {
public:
    RawSerial(PinName TxPin, PinName RxPin, int baudrate);
    void attach(Callback<void()> cb, Serial::IrqTypeT iqrType);
    void detach();
    bool readable();
    void printf(const char*, ...);
    void putc(char c);
    char getc();

private:
    Callback<void()> _cb;
};

#endif
