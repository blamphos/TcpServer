#include <cstring>
#include <cstdlib>
#include <cstdio>
#include "HttpServer.h"
#include "ESP8266.h"
#include "Parameters.h"

HttpServer::HttpServer() :
    _esp(new ESP8266), _requestType(NotDefined)
{
	_esp->initialize();
}

void HttpServer::handleMessage(message_t msg)
{
    switch (msg.event) {
    case EVENT_HTTP_REQUEST:
        //_esp->handleMessage(msg);
        //processLine();
        handleRequest();
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
    _esp->readBuffer(&buff, &len);

    if ((_requestType != NotDefined) || (strstr(buff, "\r\n\r\n") == NULL)) {
        return;

    }
    //printf(buff);

    _requestType = NotDefined;

    if (_requestType == NotDefined) {
        if (strstr(buff, "GET") != NULL) {
            _requestType = Get;
        }
        else if (strstr(buff, "POST") != NULL) {
            _requestType = Post;
        }
    }

    int volume = Parameters::instance()->current_level;
    bool auto_find = Parameters::instance()->auto_find;
    int input = Parameters::instance()->current_input;

	// Parse input data
	if (_requestType == Post) {
		parseCharValue(buff, "pot=", &volume);
		if ((volume >= 0) && (volume < 100)) {
			printf("Volume: %d\r\n", volume);
			//SystemControl::instance()->onVolumeChanged(volume);
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

        Parameters::instance()->current_level = volume;
        Parameters::instance()->auto_find = auto_find;
        Parameters::instance()->current_input = static_cast<Spdif::InputTypeT>(input);
	}

	sendResponse();
}

void HttpServer::sendResponse()
{
    char* buff;
    int len;
    _esp->readBuffer(&buff, &len);
    //char buff[BUFFER_LEN];

	// Build up response to the client
	memset(buff, '\0', len);

	FILE* fp = fopen("/local/index.html", "r");
	if (fp != NULL) {
		char ch;
		char* wp = buff;
		int line = 0;
		while ((ch = fgetc(fp)) != EOF) {
			*wp++ = ch;
			if (ch == '\n') {
				// Process line
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

				_esp->sendBuffer();
				/*iSendResult = send(ClientSocket, buff, strlen(buff), 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					break;
				}*/

				//printf("Bytes sent: %d\n", iSendResult);
				//printf(recvbuf);
				memset(buff, '\0', len);
				wp = buff;
			}
		}
	}

	fclose(fp);

	_requestType = NotDefined;
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
