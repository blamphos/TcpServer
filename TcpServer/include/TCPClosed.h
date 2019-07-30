#ifndef ESPCLOSEDSTATE_H
#define ESPCLOSEDSTATE_H

#include "TCPState.h"

class TCPClosed : public TCPState {
public:
    TCPClosed();
    virtual void onStateEnter(TCPConnection* t);
    virtual void processLine(TCPConnection* t);
    static TCPState* instance();

private:
};

#endif
