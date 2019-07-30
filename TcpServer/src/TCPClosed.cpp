#include "TCPClosed.h"
#include "TCPEstablished.h"
#include "TCPConnection.h"

TCPClosed::TCPClosed()
{

}

void TCPClosed::onStateEnter(TCPConnection* t)
{
#if 0
    t->initBuffers(ESP8266::LARGE_RX_BUF);
    t->getRxBuffer(&_buff);

    t->send("AT+CIPCLOSE=0\r\n");
#endif
}

void TCPClosed::handleMessage(TCPConnection* t, message_t msg)
{
    switch (msg.event) {
    case EVENT_SERIAL_DATA_RECEIVED:
        processLine(t);
        break;
    default:
        break;
    }
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
        changeState(t, TCPEstablished::instance());
    }
}

TCPState* TCPClosed::instance()
{
    static TCPClosed inst;
    return &inst;
}
