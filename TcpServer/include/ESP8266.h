#ifndef ESP8266_H
#define ESP8266_H

#include "mbed.h"

const char ssid[16] = "Kuhaverkko";
const char pwd[16] = "kasperi12";

class ESP8266 : public RawSerial {
public:
	ESP8266();
	~ESP8266();
	void initialize();
	void handleMessage(message_t msg);
	void readBuffer(char** buff, int* len);
	void sendBuffer();

private:
	enum ConstantsT {
		ESP_BAUD_RATE = 115200,
		SERIAL_RX_BUF_SIZE = 512,
		SERIAL_TX_BUF_SIZE = 64,
		MSG_BUF_SIZE = 16
	};

	enum ResponseTypeT {
		AT_OK = 0,
		AT_READY_TO_SEND,
		AT_IP_CONN_CLOSED,
		AT_IPD_RECEIVED
	};


	void esp_rx_isr();
	void esp_rx_flush();
	void sendNextCommand();
	void processLine();
	void queryStatus();

	DigitalOut* _esp_reset;
	Timeout _timeout;
	ResponseTypeT _expected_response;
	char _rx_buf[SERIAL_RX_BUF_SIZE];
	char _tx_buf[SERIAL_TX_BUF_SIZE];
	volatile uint32_t _buf_index;
	int _cmd_index;
};

#endif
