#include "TCPClosed.h"
#include "TCPConnection.h"
#include "TCPListen.h"

TCPClosed::TCPClosed()
{

}

void TCPClosed::onStateEnter(TCPConnection* t)
{
    t->initLargeRxBuffer();
    t->getRxBuffer(&_buff);

    t->send("AT+CIPCLOSE=0\r\n");
}

void TCPClosed::processLine(TCPConnection* t)
{
	const char* c = NULL;

    c = strstr(_buff, "ERROR");
    if (c != NULL) {
        //t->changeState(EspInitState::instance());
        return;
    }

    c = strstr(_buff, ",CLOSED");
    if (c != NULL) {
        changeState(t, TCPListen::instance());
    }
}

TCPState* TCPClosed::instance()
{
    static TCPClosed inst;
    return &inst;
}
