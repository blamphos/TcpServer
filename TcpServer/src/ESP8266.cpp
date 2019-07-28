#include "ESP8266.h"
#include "EventQueue.h"
#include "IO_mapping.h"
#include "TCPState.h"
#include "EspInitState.h"

extern Serial pc;
extern BusOut leds;

ESP8266::ESP8266() :
    RawSerial(SERIAL_TX, SERIAL_RX, ESP_BAUD_RATE),
    _state(EspInitState::instance())
{
    this->attach(callback(this, &ESP8266::rx_isr), RxIrq);
}

void ESP8266::initialize()
{
    changeState(EspInitState::instance());
}

void ESP8266::handleMessage(message_t msg)
{
    _state->handleMessage(this, msg);
}

void ESP8266::initBuffers(BufferSizeT type)
{
    memset(_buff, '\0', SERIAL_BUF_SIZE);

    switch(type) {
    case LARGE_RX_BUF:
        _rp = _buff;
        _rx_buf_len = SERIAL_LARGE_BUF;
        _wp = _buff + _rx_buf_len;
        _tx_buf_len = SERIAL_TINY_BUF;
        break;
    case LARGE_TX_BUF:
        _rp = _buff;
        _rx_buf_len = SERIAL_TINY_BUF;
        _wp = _buff + _rx_buf_len;
        _tx_buf_len = SERIAL_LARGE_BUF;
        break;
    }
}

void ESP8266::rx_flush()
{
	while (readable()) {
		getc();
	}

	_rp = _buff;
	memset(_rp, '\0', _rx_buf_len);
}

void ESP8266::rx_isr()
{
	char c = 0;
	while (readable()) {
		c = this->getc();
		pc.putc(c);
		*_rp++ = c;
#if 0
		if (c == '\n') {
			EventQueue::instance()->post(EVENT_SERIAL_DATA_RECEIVED);
		}
#endif
		if ((_rp - _buff) == _rx_buf_len) {
            _rp = _buff;
		}
	}

	EventQueue::instance()->post(EVENT_SERIAL_DATA_RECEIVED);
}

void ESP8266::getRxBuffer(char** buff, int* len)
{
    (*buff) = _buff;
    if (len != NULL) {
        *len = _rx_buf_len;
    }
}

void ESP8266::getTxBuffer(char** buff, int* len)
{
    (*buff) = _buff + _rx_buf_len;
    if (len != NULL) {
        *len = _tx_buf_len;
    }
}

void ESP8266::sendTxBuffer()
{
    int len = strlen(_buff + _rx_buf_len);
    this->printf("AT+CIPSENDBUF=0,%d\r\n", len);
}

void ESP8266::changeState(TCPState* state)
{
    _state->onStateExit(this);
    _state = state;
    _state->onStateEnter(this);
}
