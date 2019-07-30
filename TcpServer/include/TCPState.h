#ifndef ESPSTATEBASE_H
#define ESPSTATEBASE_H

#include "mbed.h"
#include "IO_mapping.h"

class TCPConnection;

class TCPState {
public:
    virtual void onStateEnter(TCPConnection* t) = 0;
    virtual void handleMessage(TCPConnection* t, message_t msg) = 0;
    virtual void processLine(TCPConnection* t) = 0;
    void open();
    void close();
    void send(const char* buff);

protected:
	enum ResponseTypeT {
		//AT_OK,
		AT_DATA_SEND_OK,
		AT_READY_TO_SEND,
		//AT_IP_CONN_CLOSED,
		AT_IPD_RECEIVED,
	};

    TCPState();
    void changeState(TCPConnection* t, TCPState* state);

    char* _buff;
	Timeout _timeout;
	ResponseTypeT _expected_response;
};

#endif
