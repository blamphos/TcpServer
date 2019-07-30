#include <cstdio>
#include "HttpServer.h"
#include "TCPConnection.h"
#include "Parameters.h"
#include "VolumeControl.h"

extern Serial pc;
extern BusOut leds;

HttpServer::HttpServer() :
    _tcp(new TCPConnection), _requestType(NotDefined)
{

}

HttpServer::~HttpServer()
{
    delete _tcp;
}

void HttpServer::handleMessage(message_t msg)
{
    switch (msg.event) {
    case EVENT_HTTP_SERVER_INIT:
    	_tcp->initialize();
    	break;
    case EVENT_HTTP_SERVER_READY:
        _tcp->open();
        break;
    case EVENT_HTTP_REQUEST:
        handleRequest();
        break;
    default:
        _tcp->handleMessage(msg);
        break;
    }
}

void HttpServer::handleRequest()
{
    char* buff;
    //_tcp->getRxBuffer(&buff);

    if (_requestType == NotDefined) {
        if (strstr(buff, "\r\n\r\n") == NULL) {
        	leds = leds | 0x8;
            return;
        }

        if (strstr(buff, "GET") != NULL) {
            _requestType = Get;
        }
        else if (strstr(buff, "POST") != NULL && strstr(buff, "pot=") != NULL) {
            _requestType = Post;
        }
        else {
            printf("Unknown request\n");
            //_esp->closeConnection();
        	pc.printf(buff);
            return;
        }
    } else {
        pc.printf("Request type already defined\n");
        return;
    }

    int volume = Parameters::instance()->current_level;
    bool auto_find = Parameters::instance()->auto_find;
    int input = Parameters::instance()->current_input;

	// Parse input data
	if (_requestType == Post) {
		parseCharValue(buff, "pot=", &volume);
		if ((VolumeControl::VALUE_MIN >= 0) && (volume <= VolumeControl::VALUE_MAX)) {
			printf("Volume: %d\r\n", volume);
			//SystemControl::instance()->onVolumeChanged(volume);
			Parameters::instance()->current_level = volume;
			EventQueue::instance()->post(EVENT_VOLUME_COMMAND, VolumeControl::Update);
		}
		else {
			return;
		}

		int temp_value = -1;
		parseCharValue(buff, "spdif=", &temp_value);

		switch (temp_value) {
		case 0:
		case 1:
		case 2:
			auto_find = false;
			input = temp_value;
			//SystemControl::instance()->onInputChanged(static_cast<Spdif::InputTypeT>(input));
			break;
		case 3:
			auto_find = !auto_find;
			break;
		default:
			break;
		}

        Parameters::instance()->auto_find = auto_find;
        Parameters::instance()->current_input = static_cast<Spdif::InputTypeT>(input);
	}

	sendResponse();
}

void HttpServer::sendResponse()
{
#if 0
    char* buff = NULL;
    int len = 0;
    int line = 0;
    if (_requestType == NotDefined) {
        return;
    }

    _tcp->initBuffers(ESP8266::LARGE_TX_BUF);
    _tcp->getTxBuffer(&buff, &len);

    FILE* fp = fopen("/local/index.htm", "rb");
    if (fp == NULL) {
        _requestType = NotDefined;
        return;
    }

    if (_requestType == Get) {
        sprintf(buff, "HTTP/1.0 200 OK\r\n\r\n");
    } else if (_requestType == Post) {
        sprintf(buff, "HTTP/1.0 201 Created\r\n\r\n");
    }

    char c = 0;
    char* wp = buff + strlen(buff);
    while ((c = fgetc(fp)) != EOF) {
        // Process next line
        *wp++ = c;
        if (c == '\n') {
            ++line;

            switch(line) {
            case 18:
                setVolumeLevel(buff, Parameters::instance()->current_level);
                break;
            case 19:
                setButtonState(buff, "coax1", Parameters::instance()->current_input == 0);
                break;
            case 20:
                setButtonState(buff, "coax2", Parameters::instance()->current_input == 1);
                break;
            case 21:
                setButtonState(buff, "opt1", Parameters::instance()->current_input == 2);
                break;
            case 22:
                setButtonState(buff, "auto", Parameters::instance()->auto_find);
                break;
            default:
                break;
            }
        }
	}

	if (fp != NULL) {
        fclose(fp);
        fp = NULL;
	}

    _requestType = NotDefined;
    _esp->sendTxBuffer();
#endif
}

void HttpServer::parseCharValue(char* buff, const char* tag, int* value)
{
	const int CHAR_BUFF_LEN = 3;

	char* c = strstr(buff, tag);
	if (c != NULL) {
		c += strlen(tag);

		char str[CHAR_BUFF_LEN] = { '\0' };
		char* wp = str;
		int len = CHAR_BUFF_LEN;

		while ((*c != '\0') && --len) {
			*wp++ = *c++;
		}

		if (value != NULL) {
			*value = atoi(str);
		}
	}
}

void HttpServer::setButtonState(char* buff, const char* tag, bool enabled)
{
	if (buff == NULL) {
		return;
	}

	char* c = strstr(buff, tag);
	if (c != NULL) {
        c += strlen(tag);

        c = strstr(c, " btn");
        if (c != NULL) {
            c += 4;
            *c = (enabled) ? '1' : '2';
        }
	}
}

void HttpServer::setVolumeLevel(char* buff, int volume)
{
	if (buff == NULL) {
		return;
	}

	char* c = strstr(buff, "value=\"");
	if (c != NULL) {
		c += 7;
		*c++ = (volume / 10) + 48;
		*c = (volume % 10) + 48;
	}
}
