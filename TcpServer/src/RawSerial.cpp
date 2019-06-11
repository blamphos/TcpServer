#include "RawSerial.h"
#include "ESP8266Simulated.h"

RawSerial::RawSerial(PinName TxPin, PinName RxPin, int baudrate)
{
	FILE* fp = fopen("/local/index.html", "r");
	if (fp == NULL) {
        printf("File 'index.html' not found!\n");
        //return -1;
	} else {
        ESP8266Simulated::instance()->attach(callback(this, RawSerial::rxIsr));
        ESP8266Simulated::instance()->start();
	}
}

void RawSerial::attach(Callback<void()> cb, Serial::IrqTypeT iqrType)
{
    _cb = cb;
}

void RawSerial::detach()
{
    _cb = NULL;
    //ESP8266Simulated::instance()->detach();
}

void RawSerial::rxIsr()
{
    int len = 0;
    char buff[2048] = {'\0'};
    ESP8266Simulated::instance()->readBuffer(buff, &len);

    printf("RX data available: %d bytes\n", len);

    char c;
    for (int i = 0; i < len; ++i) {
        c = buff[i];
        if (c == '\0') {
            break;
        }
        std::cout << c;
    }
}

bool RawSerial::readable()
{
    return false;
}

void RawSerial::printf(const char* format, ...)
{
   va_list args;
   va_start (args, format);
   vprintf (format, args);
   va_end (args);
}

void RawSerial::putc(char c)
{
}

char RawSerial::getc()
{
    return 0;
}
