#ifndef TCPLISTEN_H
#define TCPLISTEN_H

#include "TCPState.h"
#include "EventQueue.h"

class TCPConnection;

class TCPListen : public TCPState
{
public:
    TCPListen();
    virtual void onStateEnter(TCPConnection* t);
    virtual void handleMessage(TCPConnection* t, message_t msg);
    virtual void processLine(TCPConnection* t);
    static TCPState* instance();
};

#endif
