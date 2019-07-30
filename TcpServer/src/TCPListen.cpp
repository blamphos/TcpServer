#include "TCPListen.h"
#include "TCPConnection.h"
#include "TCPEstablished.h"

TCPListen::TCPListen()
{

}

void TCPListen::onStateEnter(TCPConnection* t)
{
    //t->initBuffers(ESP8266::LARGE_RX_BUF);
    //t->getRxBuffer(&_buff);
}

void TCPListen::handleMessage(TCPConnection* t, message_t msg)
{
    switch (msg.event) {
    case EVENT_SERIAL_DATA_RECEIVED:
        processLine(t);
        break;
    default:
        break;
    }
}

void TCPListen::processLine(TCPConnection* t)
{
	const char* c = NULL;

    c = strstr(_buff, "ERROR");
    /*if (c != NULL) {
        t->changeState(EspInitState::instance());
        return;
    }*/

    c = strstr(_buff, ",CLOSED");
    if (c != NULL) {
        changeState(t, TCPEstablished::instance());
    }
}

TCPState* TCPListen::instance()
{
    static TCPListen inst;
    return &inst;
}

