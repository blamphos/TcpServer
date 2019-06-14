#include "ESP8266.h"
#include "EventQueue.h"
#include "IO_mapping.h"

extern Serial pc;
extern BusOut leds;

ESP8266::ESP8266() : RawSerial(SERIAL_TX, SERIAL_RX, ESP_BAUD_RATE),
		_esp_reset(new DigitalOut(ESP8266_RST_PIN, 1)),
		_expected_response(AT_IPD_RECEIVED),
		_buf_index(0),
		_cmd_index(0)
{
    memset(_rx_buf, '\0', sizeof(_rx_buf));
}

ESP8266::~ESP8266()
{

}

void ESP8266::initialize()
{
	printf("ESP8266 initializing...\n");

	_esp_reset->write(0);
	//wait(2);
	this->attach(callback(this, &ESP8266::esp_rx_isr), RxIrq);
	_esp_reset->write(1);
	//wait(2);

	esp_rx_flush();

	_cmd_index = 0;
	//sendNextCommand();
	printf("ESP8266 initialized\n");
}

void ESP8266::esp_rx_flush()
{
	while (readable()) {
		getc();
	}
}

void ESP8266::esp_rx_isr()
{
	char c = 0;
	while (readable()) {
		c = this->getc();
		//pc.putc(c);
		_rx_buf[_buf_index] = c;
		if (c == '\n') {
			EventQueue::instance()->post(EVENT_SERIAL_CMD_RECEIVED);
		}
		++_buf_index &= 0x1FF;
	}
}

void ESP8266::queryStatus()
{
	_cmd_index = 5;
	EventQueue::instance()->post(EVENT_SERIAL_CMD_SEND);
}

void ESP8266::handleMessage()
{
	message_t msg;
	if (EventQueue::instance()->getNext(msg)) {
		switch (msg.event) {
		case EVENT_SERIAL_CMD_RECEIVED:
			processLine();
			break;
		case EVENT_SERIAL_CMD_SEND:
			_timeout.attach(callback(this, &ESP8266::sendNextCommand), 0.1);
			//sendNextCommand();
			break;
		default:
			break;
		}
	}
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
		//esp->printf("AT+GMR\r\n");
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
		//esp->printf("AT+CWLAP\r\n");
		this->printf("AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
		break;
	case 5:
		leds = 0x1;
		_expected_response = AT_OK;
		//expected_response = AT_OK;
		this->printf("AT+CIPSTART=0,\"TCP\",\"192.168.1.31\",11000\r\n");
		break;
	case 6:
		leds = 0x3;
		_expected_response = AT_OK;
		// Build query message using TX buf
		memset(_tx_buf, '\0', sizeof(_tx_buf));
		//sprintf(_tx_buf, "GET /Status HTTP/1.1\r\nHost: 192.168.1.31\r\nConnection: close\r\n\r\n");
		this->printf("AT+CIPSEND=0,%d\r\n", strlen(_tx_buf));
		break;
	case 7:
		leds = 0x7;
		_expected_response = AT_IP_CONN_CLOSED;
		this->printf(_tx_buf);
		this->printf("\r\n");
		//_timeout.attach(callback(this, &ESP8266::queryStatus), 1);
		//return;
		//expected_response = AT_OK;
		//esp->printf("AT+CIPSTATUS\r\n");
		break;
	default:
		//pc.printf("Sequence completed.\r\n");
		leds = 0;
		return;
	}

	++_cmd_index;
}

void ESP8266::processLine()
{
	const char* c = NULL;
	pc.printf("|");
    //std::cout << _rx_buf << std::endl;
	switch(_expected_response) {
	case AT_OK:
		c = strstr(_rx_buf, "OK");
		if (c != NULL) {
			EventQueue::instance()->post(EVENT_SERIAL_CMD_SEND);
		}
		break;
	case AT_READY_TO_SEND:
		c = strstr(_rx_buf, ">");
		if (c != NULL) {
			EventQueue::instance()->post(EVENT_SERIAL_CMD_SEND);
		}
		break;
	case AT_IP_CONN_CLOSED:
		c = strstr(_rx_buf, "CLOSED");
		if (c != NULL) {
			//_client->processResponse(_rx_buf);
			_timeout.attach(callback(this, &ESP8266::queryStatus), 1);
		}
		break;
	case AT_IPD_RECEIVED:
		c = strstr(_rx_buf, "+IPD");
		if (c != NULL) {
            //std::cout << _rx_buf << std::endl;
			_expected_response = AT_IP_CONN_CLOSED;
			return;
			//this->attach(NULL, Serial::RxIrq);
			//_buf_index = 0;
			//this->attach(callback(this, &ESP8266::esp_rx_isr), Serial::RxIrq);
		}
		break;
	default:
		break;
	}
}
