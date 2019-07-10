#include "EspInitState.h"
#include "EspClosedState.h"
#include "ESP8266.h"

extern Serial pc;
extern BusOut leds;

EspInitState::EspInitState() :
    _esp_reset(new DigitalOut(ESP8266_RST_PIN, 1))
{

}

void EspInitState::onStateEnter(ESP8266* esp)
{
    esp->initBuffers(ESP8266::LARGE_RX_BUF);
    esp->getRxBuffer(&_buff);

	_esp_reset->write(0);
	//wait(2);
	_esp_reset->write(1);
	//wait(2);

	//_serial->attach(callback(this, &ESP8266::esp_rx_isr), RxIrq);

	_cmd_index = 0;
	sendNextCommand(esp);
}

void EspInitState::handleMessage(ESP8266* esp, message_t msg)
{
    switch (msg.event) {
    case EVENT_SERIAL_DATA_RECEIVED:
        processLine(esp);
        break;
    case EVENT_SERIAL_CMD_SEND:
        sendNextCommand(esp);
        //_timeout.attach(callback(this, &EspInitState::sendNextCommand), 0.1);
        break;
    default:
        break;
    }
}

void EspInitState::onStateExit(ESP8266* esp)
{

}

void EspInitState::initialize()
{

}

void EspInitState::sendNextCommand(ESP8266* esp)
{
	_timeout.detach();
	esp->rx_flush();

	switch (_cmd_index) {
	case 0:
		esp->printf("AT\r\n");
		//_timeout.attach(callback(this, &ESP8266::initialize), 5);
		break;
	case 1:
		leds = 0x1;
		esp->printf("AT+RFPOWER=20\r\n");
		break;
	case 2:
		leds = 0x3;
		esp->printf("AT+CIPMUX=1\r\n");
		break;
	case 3:
		leds = 0x7;
		esp->printf("AT+CIPSERVER=1,11000\r\n");
		break;
	case 4:
		leds = 0xF;
		esp->printf("AT+CWJAP=\"%s\",\"%s\"\r\n", _ssid, _pwd);
		break;
	case 5:
		leds = 0x0;
		pc.printf("Initialization OK.\r\n");
		esp->changeState(EspClosedState::instance());
		break;
	default:
		leds = 0;
		return;
	}

	++_cmd_index;
}

void EspInitState::processLine(ESP8266* esp)
{
	const char* c = NULL;

    c = strstr(_buff, "ERROR");
    if (c != NULL) {
        EventQueue::instance()->post(EVENT_HTTP_SERVER_INIT);
        return;
    }

    c = strstr(_buff, "OK");
    if (c != NULL) {
        EventQueue::instance()->post(EVENT_SERIAL_CMD_SEND);
    }
/*	switch(_expected_response) {
	case AT_OK:
		c = strstr(_buff, "OK");
		if (c != NULL) {
            esp->rx_flush();
			EventQueue::instance()->post(EVENT_SERIAL_CMD_SEND);
		}
		break;
	case AT_DATA_SEND_OK:
		c = strstr(_buff, "OK");
		if (c != NULL) {
            esp->rx_flush();
            closeConnection();
		}
		break;
	case AT_READY_TO_SEND:
		c = strstr(_buff, ">");
		if (c != NULL) {
			esp->rx_flush();
            _expected_response = AT_DATA_SEND_OK;
            esp->puts(_buff + _rx_buf_len);
		}
		break;
	case AT_IP_CONN_CLOSED:
		c = strstr(_buff, ",CLOSED");
		if (c != NULL) {
            _expected_response = AT_IPD_RECEIVED;
            initBuffers(LARGE_RX_BUF);
            esp->rx_flush();
            leds = 0x0;
		}
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
                _timeout.attach(callback(this, &ESP8266::closeConnection), 3);
            }
        }
        if (strlen(_buff) >= _expected_data_len) {
            _timeout.detach();
            _expected_response = AT_IP_CONN_CLOSED;
            EventQueue::instance()->post(EVENT_HTTP_REQUEST);
            leds = 0x3;
        }
		break;
	default:
		break;
	}*/
}

EspStateBase* EspInitState::instance()
{
    static EspInitState inst;
    return &inst;
}
