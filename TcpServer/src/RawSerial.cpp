#include <iostream>
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
    onSocketDataReceived = cb;
}

void RawSerial::detach()
{
    onSocketDataReceived = NULL;
    //ESP8266Simulated::instance()->detach();
}

void RawSerial::rxIsr()
{
    char temp_buff[BUFFER_LEN];
    memset(temp_buff, '\0', BUFFER_LEN);
    ESP8266Simulated::instance()->readBuffer(temp_buff, &_len);

    printf("RX data available: %d bytes\n", _len);
    //printf(_buff);

    /*char c;
    for (int i = 0; i < len; ++i) {
        c = buff[i];
        if (c == '\0') {
            break;
        }
        std::cout << c;
    }*/
    //std::cout << _buff;
    char* wp = _buff;
    memset(_buff, '\0', BUFFER_LEN);
    for (int i = 0; i < _len; ++i) {
        *wp++ = temp_buff[i];
        if (i > 0 && (i % 16) == 0) {
            _rp = _buff;
            _readable = true;
            onSocketDataReceived();
            while (_readable) {
                wait_ms(10);
            }
            memset(_buff, '\0', BUFFER_LEN);
            wp = _buff;
        }
    }


	/*char ch = 0;
	while (readable()) {
		std::cout << getc();
	}*/

    memset(_buff, '\0', BUFFER_LEN);

    // Send response no here
	FILE* fp = fopen("/local/index.html", "r");
	if (fp != NULL) {
	    char c;
	    char* wp = _buff;
        while ((c = fgetc(fp)) != EOF) {
            *wp++ = c;
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
    //*_wp = c;
}

char RawSerial::getc()
{
    char c = *_rp++;
    if (*_rp == '\0') {
        _rp = _buff;
        _readable = false;
    }

    return c;
}
