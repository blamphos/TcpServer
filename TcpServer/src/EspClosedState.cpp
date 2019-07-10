#include "EspClosedState.h"
#include "EspConnectState.h"
#include "EspInitState.h"
#include "ESP8266.h"

EspClosedState::EspClosedState()
{

}

void EspClosedState::onStateEnter(ESP8266* esp)
{
    esp->initBuffers(ESP8266::LARGE_RX_BUF);
    esp->getRxBuffer(&_buff);

    esp->printf("AT+CIPCLOSE=0\r\n");
}

void EspClosedState::handleMessage(ESP8266* esp, message_t msg)
{
    switch (msg.event) {
    case EVENT_SERIAL_DATA_RECEIVED:
        processLine(esp);
        break;
    default:
        break;
    }
}

void EspClosedState::onStateExit(ESP8266* esp)
{

}

void EspClosedState::processLine(ESP8266* esp)
{
	const char* c = NULL;

    c = strstr(_buff, "ERROR");
    if (c != NULL) {
        //esp->rx_flush();
        esp->changeState(EspInitState::instance());
        return;
    }

    c = strstr(_buff, ",CLOSED");
    if (c != NULL) {
        //_expected_response = AT_IPD_RECEIVED;
        //initBuffers(LARGE_RX_BUF);
        //esp->rx_flush();
        esp->changeState(EspConnectState::instance());
    }
}

EspStateBase* EspClosedState::instance()
{
    static EspClosedState inst;
    return &inst;
}
