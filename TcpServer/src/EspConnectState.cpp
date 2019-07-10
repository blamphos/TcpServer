#include "EspConnectState.h"
#include "EspClosedState.h"
#include "ESP8266.h"

extern Serial pc;
extern BusOut leds;

EspConnectState::EspConnectState()
{

}

void EspConnectState::onStateEnter(ESP8266* esp)
{
    esp->initBuffers(ESP8266::LARGE_RX_BUF);
    esp->getRxBuffer(&_buff);

    _expected_data_len = 0;
    _expected_response = AT_IPD_RECEIVED;
}

void EspConnectState::handleMessage(ESP8266* esp, message_t msg)
{
    switch (msg.event) {
    case EVENT_SERIAL_DATA_RECEIVED:
        processLine(esp);
        break;
    default:
        break;
    }
}

void EspConnectState::onStateExit(ESP8266* esp)
{

}

void EspConnectState::timeout()
{
    pc.printf("timeout");
}

void EspConnectState::processLine(ESP8266* esp)
{
    const char* c = NULL;

    switch(_expected_response) {
    case AT_DATA_SEND_OK:
        esp->changeState(EspClosedState::instance());
        break;
    case AT_READY_TO_SEND:
        esp->getTxBuffer(&_buff);
        _expected_response = AT_DATA_SEND_OK;
        esp->puts(_buff);
        break;
    case AT_IPD_RECEIVED:
        if (_expected_data_len == 0) {
            c = strstr(_buff, "+IPD");
            if (c != NULL && strstr(_buff, ":")) {
                c += 7;
                const int CHAR_BUFF_LEN = 5;

                char str[CHAR_BUFF_LEN] = { '\0' };
                char* sp = str;
                int len = CHAR_BUFF_LEN;

                while ((*c != ':') && --len) {
                    *sp++ = *c++;
                }
                _expected_data_len = atoi(str);
                if (_expected_data_len == 0) {
                    return;
                }

                _expected_data_len += (c - _buff);
                //pc.printf("len: %d\n", _expected_data_len);
                _timeout.attach(callback(this, &EspConnectState::timeout), 3);
            }
        }

        if ((_expected_data_len > 0) && (strlen(_buff) >= _expected_data_len)) {
            _timeout.detach();
            _expected_response = AT_READY_TO_SEND;
            EventQueue::instance()->post(EVENT_HTTP_REQUEST);
        }
        break;
    default:
        break;
    }
}

EspStateBase* EspConnectState::instance()
{
    static EspConnectState inst;
    return &inst;
}