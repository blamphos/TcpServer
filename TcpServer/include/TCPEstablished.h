#ifndef ESPCONNECTSTATE_H
#define ESPCONNECTSTATE_H

#include "TCPState.h"

class TCPEstablished : public TCPState {
public:
    TCPEstablished();
    virtual void onStateEnter(TCPConnection* t);
    virtual void processLine(TCPConnection* t);
    static TCPState* instance();
};

#endif
