#include "ESP8266Simulated.h"
#include "SystemControl.h"

ESP8266Simulated::ESP8266Simulated()
{
    _listenSocket = INVALID_SOCKET;
}

void ESP8266Simulated::start()
{
    _serverThread = new std::thread(&ESP8266Simulated::serverThreadImp, this);
}

void ESP8266Simulated::stop()
{
    closesocket(_listenSocket);
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
    int volume = Parameters::instance()->current_level;
    bool auto_find = Parameters::instance()->auto_find;
    int input = Parameters::instance()->current_input;

	int iResult;
	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	HttpRequestTypeT requestType = NotDefined;

	// Receive until the peer shuts down the connection
	do {
		memset(recvbuf, '\0', recvbuflen);
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			//printf("\r\n");
			//printf("Bytes received: %d\n", iResult);
			//printf(recvbuf);
			for (int i = 0; i < strlen(recvbuf); ++i) {
                //printf("%02X ", recvbuf[i]);
                printf("%c", recvbuf[i]);
			}

			if (requestType == NotDefined) {
				if (strstr(recvbuf, "GET") != NULL) {
					requestType = Get;
				}
				else if (strstr(recvbuf, "POST") != NULL) {
					requestType = Post;
				}
			}

			if (strstr(recvbuf, "\r\n\r\n") != NULL) {
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

	// Parse input data
	if (requestType == Post) {
		parseCharValue(recvbuf, "pot=", &volume);
		if ((volume >= 0) && (volume < 100)) {
			printf("Volume: %d\r\n", volume);
			SystemControl::instance()->onVolumeChanged(volume);
		}
		else {
			return;
		}

		int temp_value = -1;
		parseCharValue(recvbuf, "spdif=", &temp_value);

		switch (temp_value) {
		case 0:
		case 1:
		case 2:
			auto_find = false;
			input = temp_value;
			SystemControl::instance()->onInputChanged(static_cast<Spdif::InputTypeT>(input));
			break;
		case 3:
			auto_find = !auto_find;
			break;
		default:
			break;
		}
	}

    Parameters::instance()->auto_find = auto_find;

	// Build up response to the client
	memset(recvbuf, '\0', recvbuflen);

	FILE* fp = fopen("/local/index.html", "r");
	if (fp != NULL) {
		char ch;
		char* wp = recvbuf;
		int line = 0;
		while ((ch = fgetc(fp)) != EOF) {
			*wp++ = ch;
			if (ch == '\n') {
				// Process line
				++line;

				switch(line) {
				case 26:
					setVolumeLevel(recvbuf, volume);
					break;
				case 28:
					setButtonState(recvbuf, input == 0);
					break;
				case 29:
					setButtonState(recvbuf, input == 1);
					break;
				case 30:
					setButtonState(recvbuf, input == 2);
					break;
				case 31:
					setButtonState(recvbuf, auto_find);
					break;
				default:
					break;
				}

				iSendResult = send(ClientSocket, recvbuf, strlen(recvbuf), 0);
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					WSACleanup();
					break;
				}
				//printf("Bytes sent: %d\n", iSendResult);
				//printf(recvbuf);
				memset(recvbuf, '\0', recvbuflen);
				wp = recvbuf;
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
	_listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (_listenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(_listenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(_listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(_listenSocket);
		WSACleanup();
		return 1;
	}

    printf("TCP server started!\n\n");

    while (true) {
        SOCKET ClientSocket = INVALID_SOCKET;
        // Accept a Connection
        while (ClientSocket == INVALID_SOCKET){
            ClientSocket = accept(_listenSocket, NULL, NULL);
        }
        std::thread t1(&ESP8266Simulated::handleConnection, this, ClientSocket);
        t1.join();
    }

	printf("server thread exit...\n");
	// No longer need server socket
	closesocket(_listenSocket);
}

ESP8266Simulated* ESP8266Simulated::instance()
{
    static ESP8266Simulated inst;
    return &inst;
}
