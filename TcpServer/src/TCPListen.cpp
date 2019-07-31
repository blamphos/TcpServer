#include "TCPListen.h"
#include "TCPConnection.h"
#include "TCPEstablished.h"

extern Serial pc;

TCPListen::TCPListen()
{

}

void TCPListen::onStateEnter(TCPConnection* t)
{
    t->initLargeRxBuffer();
    t->getRxBuffer(&_buff);

    _expected_data_len = 0;
    _expected_response = AT_IPD_RECEIVED;
}

void TCPListen::timeout()
{
    pc.printf("timeout");
}

void TCPListen::processLine(TCPConnection* t)
{
    const char* c = NULL;

    switch(_expected_response) {
/*    case AT_DATA_SEND_OK:
        t->close();
        break;
    case AT_READY_TO_SEND:
        t->getTxBuffer(&_buff);
        _expected_response = AT_DATA_SEND_OK;
        esp->puts(_buff);
        break;*/
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
                _timeout.attach(callback(this, &TCPListen::timeout), 3);
            }
        }

        if ((_expected_data_len > 0) && (strlen(_buff) >= _expected_data_len)) {
            _timeout.detach();
            //_expected_response = AT_READY_TO_SEND;
            //EventQueue::instance()->post(EVENT_HTTP_REQUEST);
            changeState(t, TCPEstablished::instance());
        }
        break;
    default:
        break;
    }
}

TCPState* TCPListen::instance()
{
    static TCPListen inst;
    return &inst;
}

