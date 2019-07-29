#ifndef ESP8266_H
#define ESP8266_H

#include "mbed.h"

class ESP8266 : public RawSerial {
public:
	enum BufferSizeT {
	    LARGE_RX_BUF,
	    LARGE_TX_BUF
	};

	ESP8266();
	void initialize();
	void getRxBuffer(char** buff, int* len = NULL);
	void getTxBuffer(char** buff, int* len = NULL);
	void initBuffers(BufferSizeT type);
	void sendTxBuffer();
	void rx_flush();
	void sendNextCommand();

private:
	enum ConstantsT {
		ESP_BAUD_RATE = 115200,
		SERIAL_BUF_SIZE = 2048,
		SERIAL_RX_BUF_INDEX_LEN_MASK = SERIAL_BUF_SIZE - 1,
		SERIAL_TINY_BUF = 32,
		SERIAL_LARGE_BUF = SERIAL_BUF_SIZE - SERIAL_TINY_BUF
	};

	void rx_isr();

    const char* _ssid = "Kuhaverkko";
    const char* _pwd = "kasperi12";

	DigitalOut* _esp_reset;
	char _buff[SERIAL_BUF_SIZE];
	char* _rp;
	char* _wp;
	int _rx_buf_len;
	int _tx_buf_len;
	int _cmd_index;
};

#endif
