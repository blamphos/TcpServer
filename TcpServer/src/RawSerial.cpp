#include <cstdarg>
#include "RawSerial.h"
#include "TcpSocketServer.h"

extern Serial pc;

RawSerial::RawSerial(PinName TxPin, PinName RxPin, int baudrate) :
    _rp(_buff), _wp(_buff), _readable(false)
{
    TcpSocketServer::instance()->attach(callback(this, RawSerial::rxIsr));
    TcpSocketServer::instance()->start();

	memset(_buff, '\0', BUFFER_LEN);
}

RawSerial::~RawSerial()
{
    TcpSocketServer::instance()->stop();
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
    TcpSocketServer::instance()->readBuffer(temp_buff, &_len);

    pc.printf("RX data available: %d bytes\n", _len);

    memset(_buff, '\0', BUFFER_LEN);
    //sprintf(_buff, "+IPD,0,%d:", _len);
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
	//ESP8266Simulated::instance()->sendBuffer(_buff, BUFFER_LEN);
}

bool RawSerial::readable()
{
    return _readable;
}


void RawSerial::printf(const char* format, ...)
{
    if (strstr(format, "AT") != NULL) {
        va_list args;
        va_start (args, format);
        pc.print_args (format, args);
        va_end (args);

        memset(_buff, '\0', BUFFER_LEN);
        Serial::str2buff(_buff, "OK\r\n");
        _rp = _buff;
        _readable = true;
        onSocketDataReceived();
    } else {
        TcpSocketServer::instance()->sendBuffer(format, strlen(format));
    }
    /*char buff[BUFFER_LEN];
    memset(buff, '\0', BUFFER_LEN);

    va_list args;
    va_start (args, format);
    vsprintf (buff, format, args);
    va_end (args);

    pc.printf(buff);*/
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
