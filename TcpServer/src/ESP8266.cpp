#include "ESP8266.h"
#include "EventQueue.h"
#include "IO_mapping.h"

extern Serial pc;
extern BusOut leds;

ESP8266::ESP8266() :
    RawSerial(SERIAL_TX, SERIAL_RX, ESP_BAUD_RATE),
    _esp_reset(new DigitalOut(ESP8266_RST_PIN, 1))
{
    this->attach(callback(this, &ESP8266::rx_isr), RxIrq);
}

void ESP8266::initialize()
{
    initBuffers(ESP8266::LARGE_RX_BUF);
    //getRxBuffer(&_buff);

	_esp_reset->write(0);
	//wait(2);
	_esp_reset->write(1);
	//wait(2);

	_cmd_index = 0;
	sendNextCommand();
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

void ESP8266::sendNextCommand()
{
	//_timeout.detach();
	rx_flush();

	switch (_cmd_index) {
	case 0:
		printf("AT\r\n");
		//_timeout.attach(callback(this, &ESP8266::initialize), 5);
		break;
	case 1:
		leds = 0x1;
		printf("AT+RFPOWER=20\r\n");
		break;
	case 2:
		leds = 0x3;
		printf("AT+CIPMUX=1\r\n");
		break;
	case 3:
		leds = 0x7;
		printf("AT+CIPSERVER=1,11000\r\n");
		break;
	case 4:
		leds = 0xF;
		printf("AT+CWJAP=\"%s\",\"%s\"\r\n", _ssid, _pwd);
		break;
	case 5:
		leds = 0x0;
		pc.printf("Initialization OK.\r\n");
		EventQueue::instance()->post(EVENT_HTTP_SERVER_READY);
		//changeState(TCPClosed::instance());
		break;
	default:
		leds = 0;
		return;
	}

	++_cmd_index;
}

void ESP8266::processLine()
{
	const char* c = NULL;

    c = strstr(_buff, "ERROR");
    if (c != NULL) {
        EventQueue::instance()->post(EVENT_HTTP_SERVER_INIT);
        return;
    }

    c = strstr(_buff, "OK");
    if (c != NULL) {
        EventQueue::instance()->post(EVENT_SERIAL_CMD_SEND);
    }
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
