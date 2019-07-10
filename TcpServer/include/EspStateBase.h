#ifndef ESPSTATEBASE_H
#define ESPSTATEBASE_H

#include "mbed.h"
#include "IO_mapping.h"

class ESP8266;

class EspStateBase {
public:
    virtual void onStateEnter(ESP8266* esp) = 0;
    virtual void handleMessage(ESP8266* esp, message_t msg) = 0;
    virtual void onStateExit(ESP8266* esp) = 0;
    virtual void processLine(ESP8266* esp) = 0;

protected:
	enum ResponseTypeT {
		//AT_OK,
		AT_DATA_SEND_OK,
		AT_READY_TO_SEND,
		//AT_IP_CONN_CLOSED,
		AT_IPD_RECEIVED,
	};

    EspStateBase();

    char* _buff;
	Timeout _timeout;
	ResponseTypeT _expected_response;
};

#endif
