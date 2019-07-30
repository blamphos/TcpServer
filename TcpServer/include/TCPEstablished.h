#ifndef ESPCONNECTSTATE_H
#define ESPCONNECTSTATE_H

#include "TCPState.h"

class TCPEstablished : public TCPState {
public:
    TCPEstablished();
    virtual void onStateEnter(TCPConnection* t);
    virtual void handleMessage(TCPConnection* t, message_t msg);
    virtual void processLine(TCPConnection* t);
    static TCPState* instance();

private:
    void timeout();

	uint32_t _expected_data_len;
};

#endif
