#include "RawSerial.h"
#include "ESP8266Simulated.h"

RawSerial::RawSerial(PinName TxPin, PinName RxPin, int baudrate) :
    _rp(_buff), _wp(_buff), _readable(false)
{
	FILE* fp = fopen("/local/index.html", "r");
	if (fp == NULL) {
        printf("File 'index.html' not found!\n");
        //return -1;
	} else {
        ESP8266Simulated::instance()->attach(callback(this, RawSerial::rxIsr));
        ESP8266Simulated::instance()->start();
	}

	memset(_buff, '\0', BUFFER_LEN);
}

RawSerial::~RawSerial()
{
    ESP8266Simulated::instance()->stop();
    std::cin.get();
}

void RawSerial::attach(Callback<void()> cb, Serial::IrqTypeT iqrType)
{
    onDataReceived = cb;
}

void RawSerial::detach()
{
    onDataReceived = NULL;
    //ESP8266Simulated::instance()->detach();
}

void RawSerial::rxIsr()
{
    int len = 0;

    memset(_buff, '\0', BUFFER_LEN);
    ESP8266Simulated::instance()->readBuffer(_buff, &len);

    //printf("RX data available: %d bytes\n", len);
    //printf(_buff);

    /*char c;
    for (int i = 0; i < len; ++i) {
        c = buff[i];
        if (c == '\0') {
            break;
        }
        std::cout << c;
    }*/
    _readable = true;
    onDataReceived();

    memset(_buff, '\0', BUFFER_LEN);

    // Send response no here
	FILE* fp = fopen("/local/index.html", "r");
	if (fp != NULL) {
	    char c;
        while ((c = fgetc(fp)) != EOF) {
            strcat(_buff, &c);
        }
        fclose(fp);
	}

	ESP8266Simulated::instance()->sendBuffer(_buff, BUFFER_LEN);
}

bool RawSerial::readable()
{
    return _readable;
}

void RawSerial::printf(const char* format, ...)
{
    char buff[BUFFER_LEN];
    memset(buff, '\0', BUFFER_LEN);

    va_list args;
    va_start (args, format);
    vsprintf (buff, format, args);
    va_end (args);

    std::cout << buff;
}

void RawSerial::putc(char c)
{
    *_wp = c;
}

char RawSerial::getc()
{
    char c = *_rp++;
    if ((*_rp == '\0') || (_rp == (_buff + BUFFER_LEN))) {
        _rp = _buff;
        _readable = false;
    }

    return c;
}
