#include <cstdarg>
#include "RawSerial.h"
#include "TcpSocketServer.h"

extern Serial pc;

RawSerial::RawSerial(PinName TxPin, PinName RxPin, int baudrate) :
    _rp(_buff), _wp(_buff), _readable(false)
{
    TcpSocketServer::instance()->attach(callback(this, &RawSerial::rxIsr));
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
    memset(_buff, '\0', BUFFER_LEN);
    TcpSocketServer::instance()->readBuffer(_buff, &_len);

    //pc.printf("RX data available: %d bytes\n", _len);

    _rp = _buff;
    _readable = true;
    onSocketDataReceived();
}

bool RawSerial::readable()
{
    return _readable;
}

void RawSerial::printf(const char* format, ...)
{
    memset(_buff, '\0', BUFFER_LEN);

    if (strstr(format, "AT") != NULL) {
#if 0
        va_list args;
        va_start (args, format);
        pc.print_args (format, args);
        va_end (args);
#endif
        memset(_buff, '\0', BUFFER_LEN);
        if (strstr(format, "CIPCLOSE") != NULL) {
            //TcpSocketServer::instance()->closeConnection(_client_socket);
            Serial::str2buff(_buff, "0,CLOSED\r\n");
        } else if (strstr(format, "CIPSENDBUF") != NULL) {
            Serial::str2buff(_buff, ">\r\n");
        } else {
            Serial::str2buff(_buff, "OK\r\n");
        }
    } else {
        TcpSocketServer::instance()->sendBuffer(format, strlen(format));
        Serial::str2buff(_buff, "0,SEND OK\r\n");
    }

    _rp = _buff;
    _readable = true;
    onSocketDataReceived();
}

void RawSerial::puts(const char* buff)
{
    printf(buff);
}

void RawSerial::putc(char c)
{
    static char* wp = _buff;

    *wp++ = c;

    if (c == '\n') {
        if (strstr(_buff, "CIPSENDBUF") != NULL) {
            Serial::str2buff(_buff, ">\r\n");
            wp = _buff;
            return;
        }
    }

    if ((wp - _buff) == BUFFER_LEN) {
        wp = _buff;
    }
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
