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
    RawSerial(PinName TxPin, PinName RxPin, int baudrate) {}

    void attach(Callback<void()> cb, Serial::IrqTypeT iqrType) {
        _cb = cb;
    }

    template<typename T>
    void attach(T* tptr, void (T::*mptr)(), Serial::IrqTypeT iqrType)
    {
        _cb = callback(tptr, mptr);
    }

    void detach()
    {
        _cb = NULL;
    }
    bool readable() { return true; }
    void printf(const char*, ...) {}
    void putc(char c) {}
    char getc()
    {
        return 0;
    }

private:
    Callback<void()> _cb;
};

#endif
