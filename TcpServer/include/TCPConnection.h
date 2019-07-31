#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "EventQueue.h"

class TCPState;
class ESP8266;

class TCPConnection {
public:
    TCPConnection();
	void handleMessage(message_t msg);
	void initialize();
    void listen();
    void close();
    void send(const char* buff);
    void initLargeRxBuffer();
    void initLargeTxBuffer();
	void getRxBuffer(char** buff, int* len = NULL);
	void getTxBuffer(char** buff, int* len = NULL);
	void sendTxBuffer();
	//void rx_flush();

private:
    friend class TCPState;
	void changeState(TCPState* state);

	TCPState* _state;
	ESP8266* _esp;
};

#endif
