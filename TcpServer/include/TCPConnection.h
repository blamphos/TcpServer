#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "EventQueue.h"

class TCPState;

class TCPConnection
{
public:
    TCPConnection();
	void handleMessage(message_t msg);
	void initialize();
	void getRxBuffer(char** buff, int* len = NULL);
	void getTxBuffer(char** buff, int* len = NULL);
	//void initBuffers(BufferSizeT type);
	void sendTxBuffer();
	void rx_flush();
	void changeState(TCPState* state);

    private:
};

#endif
