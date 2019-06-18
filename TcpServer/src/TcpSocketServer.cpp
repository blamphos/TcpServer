#include <thread>
#include "TcpSocketServer.h"

TcpSocketServer::TcpSocketServer()
{
    _listen_socket = INVALID_SOCKET;
}

void TcpSocketServer::attach(Callback<void()> cb)
{
    onDataReceived = cb;
}

void TcpSocketServer::detach()
{
    onDataReceived = NULL;
}

void TcpSocketServer::readBuffer(char* buff, int* len)
{
    //memcpy(buff, _buffer, DEFAULT_BUFLEN);
    memset(buff, '\0', DEFAULT_BUFLEN);

    sprintf(buff, "+IPD,0,%d:%s", strlen(_buffer), _buffer);
    (*len) = strlen(buff);
}

void TcpSocketServer::sendBuffer(const char* buff, int len)
{
    int iSendResult;

    memset(_buffer, '\0', DEFAULT_BUFLEN);

    // REMOVE THIS
#if 0
	FILE* fp = fopen("/local/index.html", "r");
	if (fp != NULL) {
	    char c;
	    char* wp = _buffer;
        while ((c = fgetc(fp)) != EOF) {
            *wp++ = c;
        }
        fclose(fp);
	}
#else
    memcpy(_buffer, buff, len);
#endif

    //printf(_buffer);
    iSendResult = send(_client_socket, _buffer, strlen(_buffer), 0);
    if (iSendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        //closesocket(ClientSocket);
        //WSACleanup();
        //break;
    }
}

void TcpSocketServer::start()
{
    new std::thread(&TcpSocketServer::serverThreadImp, this);
}

void TcpSocketServer::stop()
{
    closesocket(_listen_socket);
    WSACleanup();
    printf("TCP server stopped.\n");
}

void TcpSocketServer::closeConnection()
{
    // shutdown the connection since we're done
	int iResult = shutdown(_client_socket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
	}

	closesocket(_client_socket);

	_connection_handled = true;
}

void TcpSocketServer::handleConnection(SOCKET socket)
{
    _connection_handled = false;
    _client_socket = socket;
	int iResult;

	// Receive until the peer shuts down the connection
	do {
		memset(_buffer, '\0', DEFAULT_BUFLEN);
		iResult = recv(_client_socket, _buffer, DEFAULT_BUFLEN, 0);
		if (iResult > 0) {
			//printf("\r\n");
			//printf("Bytes received: %d\n", iResult);
			//printf(_buffer);
			/*for (unsigned int i = 0; i < strlen(_buffer); ++i) {
                //printf("%02X ", recvbuf[i]);
                printf("%c", _buffer[i]);
			}*/
			break;
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			closesocket(_client_socket);
			//WSACleanup();
			return;
		}

	} while (iResult > 0);

	onDataReceived();

	while (!_connection_handled) {
        //wait_ms(10);
	}
}

int TcpSocketServer::serverThreadImp()
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

    SOCKET socket;
    while (true) {
        socket = INVALID_SOCKET;
        // Accept a Connection
        while (socket == INVALID_SOCKET){
            socket = accept(_listen_socket, NULL, NULL);
        }

        std::thread t1(&TcpSocketServer::handleConnection, this, socket);
        t1.join();
    }

	printf("server thread exit...\n");
	// No longer need server socket
	closesocket(_listen_socket);
}

TcpSocketServer* TcpSocketServer::instance()
{
    static TcpSocketServer inst;
    return &inst;
}
