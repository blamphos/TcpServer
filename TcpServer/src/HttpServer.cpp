#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "HttpServer.h"
#include "ESP8266.h"
#include "Parameters.h"
#include "VolumeControl.h"

HttpServer::HttpServer() :
    _esp(new ESP8266), _requestType(NotDefined)
{
	_esp->initialize();
}

HttpServer::~HttpServer()
{
    delete _esp;
}

void HttpServer::handleMessage(message_t msg)
{
    switch (msg.event) {
    case EVENT_HTTP_REQUEST:
        handleRequest();
        break;
    case EVENT_HTTP_RESPONSE:
        sendResponse();
        break;
    default:
        _esp->handleMessage(msg);
        break;
    }
}

void HttpServer::handleRequest()
{
    char* buff;
    int len;
    _esp->getRxBuffer(&buff, &len);

    if (_requestType == NotDefined) {
        if (strstr(buff, "\r\n\r\n") == NULL) {
            return;
        }

        if (strstr(buff, "GET") != NULL) {
            _requestType = Get;
        }
        else if (strstr(buff, "POST") != NULL && strstr(buff, "pot=") != NULL) {
            _requestType = Post;
        }
        else {
            return;
        }
    } else {
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

	sendResponse(true);
}

void HttpServer::sendResponse(bool firstSegment)
{
    static FILE* fp = NULL;
    static char* buff = NULL;
    static int line = 0;
    static int len = -1;

    if (_requestType == NotDefined) {
        return;
    }

    if (firstSegment) {
        fp = fopen("/local/index.html", "r");
        if (fp == NULL) {
            return;
        }

        line = 0;
    }

    _esp->getTxBuffer(&buff, &len);
    memset(buff, '\0', len);

    char ch;
    char* wp = buff;
    while ((ch = fgetc(fp)) != EOF) {
        *wp++ = ch;

        // Process next line
        if (ch == '\n') {
            ++line;

            switch(line) {
            case 26:
                setVolumeLevel(buff, Parameters::instance()->current_level);
                break;
            case 28:
                setButtonState(buff, Parameters::instance()->current_input == 0);
                break;
            case 29:
                setButtonState(buff, Parameters::instance()->current_input == 1);
                break;
            case 30:
                setButtonState(buff, Parameters::instance()->current_input == 2);
                break;
            case 31:
                setButtonState(buff, Parameters::instance()->auto_find);
                break;
            default:
                break;
            }

            _esp->sendTxBuffer();
            return;
        }
	}

	if (fp != NULL) {
        fclose(fp);
        fp = NULL;
        _requestType = NotDefined;
        _esp->closeConnection();
	}
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

void HttpServer::setButtonState(char* buff, bool enabled)
{
	if (buff == NULL) {
		return;
	}

	char* c = strstr(buff, " button");
	if (c != NULL) {
		c += 7;
		*c = (enabled) ? '1' : '2';
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
