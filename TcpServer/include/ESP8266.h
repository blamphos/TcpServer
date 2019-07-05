#ifndef ESP8266_H
#define ESP8266_H

#include "mbed.h"

const char ssid[16] = "Kuhaverkko";
const char pwd[16] = "kasperi12";

class ESP8266 : public RawSerial {
public:
	enum BufferSizeT {
	    LARGE_RX_BUF,
	    LARGE_TX_BUF
	};

	ESP8266();
	void initialize();
	void handleMessage(message_t msg);
	void getRxBuffer(char** buff, int* len);
	void getTxBuffer(char** buff, int* len);
	void initBuffers(BufferSizeT type);
	void sendTxBuffer();
	void closeConnection();

private:
	enum ConstantsT {
		ESP_BAUD_RATE = 115200,
		SERIAL_BUF_SIZE = 2048,
		SERIAL_RX_BUF_INDEX_LEN_MASK = SERIAL_BUF_SIZE - 1,
		SERIAL_TINY_BUF = 32,
		SERIAL_LARGE_BUF = SERIAL_BUF_SIZE - SERIAL_TINY_BUF
	};

	enum ResponseTypeT {
	    AT_NACK,
		AT_OK,
		AT_DATA_SEND_OK,
		AT_READY_TO_SEND,
		AT_IP_CONN_CLOSED,
		AT_IPD_RECEIVED,
	};

	void esp_rx_isr();
	void esp_rx_flush();
	void sendNextCommand();
	void processLine();

	DigitalOut* _esp_reset;
	Timeout _timeout;
	ResponseTypeT _expected_response;
	char _buff[SERIAL_BUF_SIZE];
	char* _rp;
	char* _wp;
	int _rx_buf_len;
	int _tx_buf_len;
	int _cmd_index;
	uint32_t _expected_data_len;
};

#endif
