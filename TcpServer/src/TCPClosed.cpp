#include "TCPClosed.h"
#include "TCPEStablished.h"
#include "EspInitState.h"
#include "ESP8266.h"

TCPClosed::TCPClosed()
{

}

void TCPClosed::onStateEnter(ESP8266* esp)
{
    esp->initBuffers(ESP8266::LARGE_RX_BUF);
    esp->getRxBuffer(&_buff);

    esp->printf("AT+CIPCLOSE=0\r\n");
}

void TCPClosed::handleMessage(ESP8266* esp, message_t msg)
{
    switch (msg.event) {
    case EVENT_SERIAL_DATA_RECEIVED:
        processLine(esp);
        break;
    default:
        break;
    }
}

void TCPClosed::onStateExit(ESP8266* esp)
{

}

void TCPClosed::processLine(ESP8266* esp)
{
	const char* c = NULL;

    c = strstr(_buff, "ERROR");
    if (c != NULL) {
        esp->changeState(EspInitState::instance());
        return;
    }

    c = strstr(_buff, ",CLOSED");
    if (c != NULL) {
        esp->changeState(TCPEStablished::instance());
    }
}

TCPState* TCPClosed::instance()
{
    static TCPClosed inst;
    return &inst;
}
