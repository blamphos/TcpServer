#include "RawSerial.h"
#include "ESP8266Simulated.h"

extern Serial pc;

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
	//Serial::
}

RawSerial::~RawSerial()
{
    ESP8266Simulated::instance()->stop();
    //std::cin.get();
}

void RawSerial::attach(Callback<void()> cb, IrqTypeT iqrType)
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

    //printf("RX data available: %d bytes\n", _len);

    memset(_buff, '\0', BUFFER_LEN);
    sprintf(_buff, "+IPD,0,%d:", _len);
    char* wp = _buff + strlen(_buff);

    for (int i = 0; i < _len; ++i) {
        *wp++ = temp_buff[i];
        if (temp_buff[i] == '\n') {
            _rp = _buff;
            _readable = true;
            onSocketDataReceived();
            wait_ms(10);
            while (_readable) {
                wait_ms(10);
            }
            memset(_buff, '\0', BUFFER_LEN);
            wp = _buff;
        }
    }

    // Semd response to client
    memset(_buff, '\0', BUFFER_LEN);
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

    //std::cout << buff;
}

void RawSerial::putc(char c)
{
    //*_wp = c;
}

char RawSerial::getc()
{
    wait_ms(1);
    char c = *_rp++;
    if (*_rp == '\0') {
        _rp = _buff;
        _readable = false;
    }

    return c;
}
