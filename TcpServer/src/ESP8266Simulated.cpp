#include "ESP8266Simulated.h"

ESP8266Simulated::ESP8266Simulated()
{
    _listen_socket = INVALID_SOCKET;
}

void ESP8266Simulated::attach(Callback<void()> cb)
{
    onDataReceived = cb;
}

void ESP8266Simulated::detach()
{
    onDataReceived = NULL;
}

void ESP8266Simulated::getBuffer(char* buff, int* len)
{
    memcpy(buff, _buffer, DEFAULT_BUFLEN);
    (*len) = DEFAULT_BUFLEN;
}

void ESP8266Simulated::start()
{
    _serverThread = new std::thread(&ESP8266Simulated::serverThreadImp, this);
}

void ESP8266Simulated::stop()
{
    closesocket(_listen_socket);
    WSACleanup();
}

void ESP8266Simulated::parseCharValue(char* buff, const char* tag, int* value)
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

void ESP8266Simulated::setButtonState(char* buff, bool enabled)
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

void ESP8266Simulated::setVolumeLevel(char* buff, int volume)
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

void ESP8266Simulated::handleConnection(SOCKET ClientSocket)
{
    int volume = 0;//Parameters::instance()->current_level;
    bool auto_find = true;//Parameters::instance()->auto_find;
    int input = 0;//Parameters::instance()->current_input;

	int iResult;
	int iSendResult;

	HttpRequestTypeT requestType = NotDefined;

	// Receive until the peer shuts down the connection
	do {
		memset(_buffer, '\0', DEFAULT_BUFLEN);
		iResult = recv(ClientSocket, _buffer, DEFAULT_BUFLEN, 0);
		if (iResult > 0) {
			//printf("\r\n");
			//printf("Bytes received: %d\n", iResult);
			//printf(recvbuf);
			/*for (unsigned int i = 0; i < strlen(_buffer); ++i) {
                //printf("%02X ", recvbuf[i]);
                printf("%c", _buffer[i]);
			}*/

			if (requestType == NotDefined) {
				if (strstr(_buffer, "GET") != NULL) {
					requestType = Get;
				}
				else if (strstr(_buffer, "POST") != NULL) {
					requestType = Post;
				}
			}

			if (strstr(_buffer, "\r\n\r\n") != NULL) {
                //printf("HTTP header received\n");
				break;
			}
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			//WSACleanup();
			return;
		}

	} while (iResult > 0);

	onDataReceived();

	// Parse input data
	if (requestType == Post) {
		parseCharValue(_buffer, "pot=", &volume);
		if ((volume >= 0) && (volume < 100)) {
			printf("Volume: %d\r\n", volume);
			//SystemControl::instance()->onVolumeChanged(volume);
		}
		else {
			return;
		}

		int temp_value = -1;
		parseCharValue(_buffer, "spdif=", &temp_value);

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
	}

    //Parameters::instance()->auto_find = auto_find;

	// Build up response to the client
	memset(_buffer, '\0', DEFAULT_BUFLEN);

	FILE* fp = fopen("/local/index.html", "r");
	if (fp != NULL) {
		char ch;
		char* wp = _buffer;
		int line = 0;
		while ((ch = fgetc(fp)) != EOF) {
			*wp++ = ch;
			if (ch == '\n') {
				// Process line
				++line;

				switch(line) {
				case 26:
					setVolumeLevel(_buffer, volume);
					break;
				case 28:
					setButtonState(_buffer, input == 0);
					break;
				case 29:
					setButtonState(_buffer, input == 1);
					break;
				case 30:
					setButtonState(_buffer, input == 2);
					break;
				case 31:
					setButtonState(_buffer, auto_find);
					break;
				default:
					break;
				}

				iSendResult = send(ClientSocket, _buffer, strlen(_buffer), 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					break;
				}
				//printf("Bytes sent: %d\n", iSendResult);
				//printf(recvbuf);
				memset(_buffer, '\0', DEFAULT_BUFLEN);
				wp = _buffer;
			}
		}
	}

	fclose(fp);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
	}

	closesocket(ClientSocket);
}

int ESP8266Simulated::serverThreadImp()
{
	WSADATA wsaData;
	int iResult;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create a SOCKET for connecting to server
	_listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (_listen_socket == INVALID_SOCKET) {
		printf("socket failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(_listen_socket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(_listen_socket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(_listen_socket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(_listen_socket);
		WSACleanup();
		return 1;
	}

    printf("TCP server started!\n\n");

    while (true) {
        _client_socket = INVALID_SOCKET;
        // Accept a Connection
        while (_client_socket == INVALID_SOCKET){
            _client_socket = accept(_listen_socket, NULL, NULL);
        }
        std::thread t1(&ESP8266Simulated::handleConnection, this, _client_socket);
        t1.join();
    }

	printf("server thread exit...\n");
	// No longer need server socket
	closesocket(_listen_socket);
}

ESP8266Simulated* ESP8266Simulated::instance()
{
    static ESP8266Simulated inst;
    return &inst;
}

