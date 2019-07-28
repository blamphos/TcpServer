#ifndef ESPCLOSEDSTATE_H
#define ESPCLOSEDSTATE_H

#include "TCPState.h"

class TCPClosed : public TCPState {
public:
    TCPClosed();
    virtual void onStateEnter(ESP8266* esp);
    virtual void handleMessage(ESP8266* esp, message_t msg);
    virtual void onStateExit(ESP8266* esp);
    virtual void processLine(ESP8266* esp);
    static TCPState* instance();

private:
};

#endif
