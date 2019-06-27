#include "ESP8266.h"
#include "EventQueue.h"
#include "IO_mapping.h"

extern Serial pc;
extern BusOut leds;

ESP8266::ESP8266() : RawSerial(SERIAL_TX, SERIAL_RX, ESP_BAUD_RATE),
		_esp_reset(new DigitalOut(ESP8266_RST_PIN, 1)),
		_expected_response(AT_OK),
		_buf_index(0),
		_cmd_index(0)
{
    memset(_rx_buf, '\0', sizeof(_rx_buf));
}

void ESP8266::handleMessage(message_t msg)
{
    switch (msg.event) {
    case EVENT_SERIAL_DATA_RECEIVED:
        processLine();
        break;
    case EVENT_SERIAL_DATA_SEND:
        _expected_response = AT_DATA_SEND_OK;
        this->printf(_tx_buf);
        break;
    case EVENT_SERIAL_CMD_SEND:
        _timeout.attach(callback(this, &ESP8266::sendNextCommand), 0.1);
        break;
    default:
        break;
    }
}

void ESP8266::initialize()
{
	_esp_reset->write(0);
	//wait(2);
	_esp_reset->write(1);
	//wait(2);

	this->attach(callback(this, &ESP8266::esp_rx_isr), RxIrq);
	esp_rx_flush();

	_cmd_index = 0;
	sendNextCommand();
}

void ESP8266::closeConnection()
{
    _expected_response = AT_IP_CONN_CLOSED;
    this->printf("AT+CIPCLOSE=0\r\n");
}

void ESP8266::getRxBuffer(char** buff, int* len)
{
    (*buff) = _rx_buf;
    *len = SERIAL_RX_BUF_SIZE;
}

void ESP8266::getTxBuffer(char** buff, int* len)
{
    (*buff) = _tx_buf;
    *len = SERIAL_TX_BUF_SIZE;
}

void ESP8266::sendTxBuffer()
{
    esp_rx_flush();
    _expected_response = AT_READY_TO_SEND;
    //pc.printf("AT+CIPSENDBUF=0,%d\r\n", strlen(_tx_buf));
    this->printf("AT+CIPSENDBUF=0,%d\r\n", strlen(_tx_buf));
}

void ESP8266::esp_rx_flush()
{
	while (readable()) {
		getc();
	}

	memset(_rx_buf, '\0', SERIAL_RX_BUF_SIZE);
	_buf_index = 0;
	_expected_data_len = 0;
}

void ESP8266::esp_rx_isr()
{
	char c = 0;
	while (readable()) {
		c = this->getc();
		//pc.putc(c);
		_rx_buf[_buf_index] = c;
#if 0
		if (c == '\n') {
			EventQueue::instance()->post(EVENT_SERIAL_DATA_RECEIVED);
		}
#endif
		++_buf_index &= 0x3FF;
	}

	EventQueue::instance()->post(EVENT_SERIAL_DATA_RECEIVED);
}

void ESP8266::sendNextCommand()
{
	_timeout.detach();
	memset(_rx_buf, '\0', sizeof(_rx_buf));
	_buf_index = 0;

	switch (_cmd_index) {
	case 0:
		_expected_response = AT_OK;
		this->printf("AT\r\n");
		_timeout.attach(callback(this, &ESP8266::initialize), 5);
		break;
	case 1:
		leds = 0x1;
		_expected_response = AT_OK;
		this->printf("AT+RFPOWER=20\r\n");
		break;
	case 2:
		leds = 0x3;
		_expected_response = AT_OK;
		this->printf("AT+CIPMUX=1\r\n");
		break;
	case 3:
		leds = 0x7;
		_expected_response = AT_OK;
		this->printf("AT+CIPSERVER=1,11000\r\n");
		break;
	case 4:
		leds = 0xF;
		_expected_response = AT_OK;
		this->printf("AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
		break;
	case 5:
		leds = 0x0;
		_expected_response = AT_IPD_RECEIVED;
		pc.printf("Initialization OK.\r\n");
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
    c = strstr(_rx_buf, "ERROR");
    if (c != NULL) {
        esp_rx_flush();
        _expected_response = AT_IPD_RECEIVED;
        return;
    }

	switch(_expected_response) {
	case AT_OK:
		c = strstr(_rx_buf, "OK");
		if (c != NULL) {
            esp_rx_flush();
			EventQueue::instance()->post(EVENT_SERIAL_CMD_SEND);
		}
		break;
	case AT_DATA_SEND_OK:
		c = strstr(_rx_buf, "OK");
		if (c != NULL) {
            esp_rx_flush();
			EventQueue::instance()->post(EVENT_HTTP_RESPONSE);
		}
		break;
	case AT_READY_TO_SEND:
		c = strstr(_rx_buf, ">");
		if (c != NULL) {
			esp_rx_flush();
            _expected_response = AT_DATA_SEND_OK;
            this->puts(_tx_buf);
		}
		break;
	case AT_IP_CONN_CLOSED:
		c = strstr(_rx_buf, ",CLOSED");
		if (c != NULL) {
            esp_rx_flush();
            _expected_response = AT_IPD_RECEIVED;
            leds = 0x0;
		}
		break;
	case AT_IPD_RECEIVED:
        if (_expected_data_len == 0) {
            c = strstr(_rx_buf, "+IPD");
            if (c != NULL && strstr(_rx_buf, ":")) {
                c += 7;
                const int CHAR_BUFF_LEN = 5;

                char str[CHAR_BUFF_LEN] = { '\0' };
                char* wp = str;
                int len = CHAR_BUFF_LEN;

                while ((*c != ':') && --len) {
                    *wp++ = *c++;
                }
                _expected_data_len = atoi(str);// + (c - _rx_buf);
                if (_expected_data_len == 0) {
                	return;
                }

                _expected_data_len += (c - _rx_buf);
                //pc.printf("len: %d\n", _expected_data_len);
                _timeout.attach(callback(this, &ESP8266::closeConnection), 3);
                //this->attach(NULL, Serial::RxIrq);
                //_buf_index = 0;
                //this->attach(callback(this, &ESP8266::esp_rx_isr), Serial::RxIrq);
            }
        }
        if (_expected_data_len != 0 && _buf_index >= _expected_data_len) {
            _timeout.detach();
            _expected_response = AT_IP_CONN_CLOSED;
            EventQueue::instance()->post(EVENT_HTTP_REQUEST);
            leds = 0x3;
            //pc.printf(_rx_buf);
            //pc.printf("Send HTTP request\n");
        }
		break;
	default:
		break;
	}
}
