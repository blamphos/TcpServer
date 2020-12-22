#include <iostream>
#include <fstream>
#include <cstring>
//#include <unistd.h>		//write
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include "TcpSocketServer.h"

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")
#pragma warning(disable : 4996)

TcpSocketServer::TcpSocketServer(uint16_t port) :
	_port(port),
    _listenSocket(INVALID_SOCKET)
{
	WSADATA wsaData;
	struct addrinfo hints;

	// Initialize Winsock
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	char portStr[10] = { 0 };
	sprintf(portStr, "%d", _port);
	iResult = getaddrinfo(NULL, portStr, &hints, &_server);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	_listenSocket = socket(_server->ai_family, _server->ai_socktype, _server->ai_protocol);
	if (_listenSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(_server);
		WSACleanup();
		return;
	}

	start();
}

TcpSocketServer::~TcpSocketServer()
{
	closesocket(_listenSocket);
	WSACleanup();
}

void TcpSocketServer::shutdown()
{
	struct addrinfo *result, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	int iResult = getaddrinfo("127.0.0.1", "80", &hints, &result);
	if (iResult != 0) {
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Create a SOCKET for connecting to server
	SOCKET ConnectSocket = socket(result->ai_family, result->ai_socktype,
		result->ai_protocol);
	if (ConnectSocket == INVALID_SOCKET) {
		printf("socket failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		return;
	}

	// Connect to server.
	iResult = connect(ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("connect failed with error: %d\n", iResult);
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
		return;
	}

	const char* buffer = "shutdown";
	iResult = send(ConnectSocket, buffer, (int)strlen(buffer), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return;
	}

	closesocket(ConnectSocket);
}

void TcpSocketServer::run()
{
	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;

	// Toggles local address reuse
	/*const char reuse = 1;
	if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(char)) < 0) {
		puts("setsockopt failed");
		return;
	}*/

	// Setup the TCP listening socket
	int iResult = bind(_listenSocket, _server->ai_addr, (int)_server->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(_server);
		closesocket(_listenSocket);
		WSACleanup();
		return;
	}

	freeaddrinfo(_server);

	iResult = listen(_listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		printf("listen failed with error: %d\n", WSAGetLastError());
		closesocket(_listenSocket);
		WSACleanup();
		return;
	}

	// Accept and incoming connection
	printf("Waiting for incoming connections on port %d\n", _port);

	while (!stopRequested())
	{
		int clientSocket = accept(_listenSocket, NULL, NULL);
		if (clientSocket < 0) {
			puts("accept() failed");
			closesocket(_listenSocket);
			break;
		}

		//unsigned int timeout_ms = 250;
		//setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout_ms, sizeof(unsigned int));
		handleConnection(clientSocket);
	}
	
	puts("TCP server thread exit");
}
