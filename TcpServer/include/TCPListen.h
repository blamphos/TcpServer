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
    virtual void processLine(TCPConnection* t);
    static TCPState* instance();

private:
    void timeout();
	uint32_t _expected_data_len;
};

#endif
