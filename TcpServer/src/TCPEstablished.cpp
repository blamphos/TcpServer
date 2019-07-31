#include "TCPEstablished.h"
#include "TCPClosed.h"
#include "TCPConnection.h"

extern Serial pc;
extern BusOut leds;

TCPEstablished::TCPEstablished()
{

}

void TCPEstablished::onStateEnter(TCPConnection* t)
{
    t->initLargeTxBuffer();
    t->getRxBuffer(&_buff);

    _expected_response = AT_READY_TO_SEND;
}

void TCPEstablished::processLine(TCPConnection* t)
{
    switch(_expected_response) {
    case AT_DATA_SEND_OK:
        changeState(t, TCPClosed::instance());
        break;
    case AT_READY_TO_SEND:
        t->getTxBuffer(&_buff);
        _expected_response = AT_DATA_SEND_OK;
        //t->puts(_buff);
        break;
    default:
        break;
    }
}

TCPState* TCPEstablished::instance()
{
    static TCPEstablished inst;
    return &inst;
}
