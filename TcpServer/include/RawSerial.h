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
    ~RawSerial();
    void attach(Callback<void()> cb, Serial::IrqTypeT iqrType);
    void detach();
    bool readable();
    void printf(const char* format, ...);
    void putc(char c);
    char getc();
    Callback<void()> onDataReceived;

private:
    enum ConstantsT {
        BUFFER_LEN = 2048
    };

    void rxIsr();

    char* _rp;
    char* _wp;
    bool _readable;
    char _buff[BUFFER_LEN];
};

#endif
