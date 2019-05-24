#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
#pragma warning(disable : 4996)

#define DEFAULT_BUFLEN	2048
#define DEFAULT_PORT	"80"

bool readFile(const char* file_name, char* in_buff, int len)
{
	FILE* fp = fopen(file_name, "r"); // read mode

	if (fp == NULL) {
		perror("Error while opening the file.\n");
		return false;
	}

	memset(in_buff, '\0', len);

	char ch;
	while ((ch = fgetc(fp)) != EOF) {
		*in_buff++ = ch;
		if (--len == 0) {
			break;
		}
	}

	fclose(fp);
	return true;
}

void parseCharValue(char* buff, const char* tag, int* value)
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

char* setButtonState(char *buff, bool enabled)
{
	if (buff == NULL) {
		return buff;
	}

	char* c = strstr(buff, "button1 button");
	if (c != NULL) {
		c += 14;
		*c = (enabled) ? '1' : '2';
	}

	return c;
}

int handleConnection(int& volume, int& input, bool& auto_find)
{
	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL;
	struct addrinfo hints;

	int iSendResult;
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;
	char txbuf[DEFAULT_BUFLEN];
	int txbuflen = DEFAULT_BUFLEN;

	if (!readFile("..\\index.html", txbuf, txbuflen)) {
		//printf(recvbuf);
		//printf("\r\n\r\nLen: %d bytes", strlen(recvbuf));
		return 2;
	}

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
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	freeaddrinfo(result);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// No longer need server socket
	closesocket(ListenSocket);

	// Receive until the peer shuts down the connection
	do {
		memset(recvbuf, '\0', recvbuflen);
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("\r\n");
			printf("Bytes received: %d\n", iResult);
			//printf(recvbuf);
			printf("\r\n");

			parseCharValue(recvbuf, "pot=", &volume);
			if ((volume >= 0) && (volume < 100)) {
				printf("Volume: %d\r\n", volume);
			}
			else {
				return 2;
			}

			int temp_value = -1;
			parseCharValue(recvbuf, "spdif=", &temp_value);

			switch (temp_value) {
			case 0:
			case 1:
			case 2:
				input = temp_value;
				break;
			case 3:
				auto_find = !auto_find;
				if (auto_find) {
					input = (input + 1) % 3;
				}
				break;
			default:
				break;
			}

			char* c = setButtonState(txbuf, input == 0);
			c = setButtonState(c, input == 1);
			c = setButtonState(c, input == 2);
			c = setButtonState(c, auto_find);
			
			c = strstr(txbuf, "value=\"");
			if (c != NULL) {
				c += 7;
				*c++ = (volume / 10) + 48;
				*c = (volume % 10) + 48;
			}		

			iSendResult = send(ClientSocket, txbuf, strlen(txbuf), 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
			break;

#if 0
			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed with error: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
#endif
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);

	// shutdown the connection since we're done
	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}

int __cdecl main(void)
{
	static int volume = 26;
	static int input = 0;
	static bool auto_find = true;

	printf("TCP server started!\n\n");
	while (handleConnection(volume, input, auto_find) == 0) {

	}
	return 0;
}
