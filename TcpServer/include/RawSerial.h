#ifndef RAW_SERIAL
#define RAW_SERIAL

#include "mbed.h"

class RawSerial {
public:
    enum IrqTypeT {
        RxIrq,
        TxIrq,
    };

    RawSerial(PinName TxPin, PinName RxPin, int baudrate);
    virtual ~RawSerial();
    void attach(Callback<void()> cb, IrqTypeT iqrType);
    void detach();
    bool readable();
    void printf(const char* format, ...);
    void puts(const char* buff);
    void putc(char c);
    char getc();
    Callback<void()> onSocketDataReceived;

private:
    enum ConstantsT {
        BUFFER_LEN = 4096
    };

    void rxIsr();

    int _len;
    char* _rp;
    char* _wp;
    bool _readable;
    char _buff[BUFFER_LEN];
};

#endif
