#include <thread>
#include <time.h>
#include "TcpSocketServer.h"
#include "HttpResponse.h"
#include "HttpServer.h"

std::mutex TcpSocketServer::_mutex;

TcpSocketServer::TcpSocketServer() :
    _listen_socket(INVALID_SOCKET),
    _client_socket(INVALID_SOCKET),
    _is_running(false)
{
	//memset(_buffer, '\0', DEFAULT_BUFLEN);
	char t[80];
	getGmtDateTime(t, 80);
	puts(t);
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
    if (_client_socket != INVALID_SOCKET) {
        //sprintf(buff, "0,CONNECT\r\n\r\n+IPD,0,%d:%s", strlen(_buffer), _buffer);
    } else {
        //memcpy(buff, _buffer, strlen(_buffer));
    }

    (*len) = strlen(buff);
}

void TcpSocketServer::sendBuffer(const char* buff, int len)
{
#if 0
    memset(_buffer, '\0', DEFAULT_BUFLEN);
    memcpy(_buffer, buff, len);

    //printf(_buffer);
    int iSendResult = send(_client_socket, _buffer, strlen(_buffer), 0);
    if (iSendResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        //closesocket(ClientSocket);
        //WSACleanup();
        //break;
    }
#endif
}

void TcpSocketServer::start()
{
    new std::thread(&TcpSocketServer::serverThreadImp, this);
}

void TcpSocketServer::stop()
{
    //closeConnection();

    _is_running = false;
    closesocket(_listen_socket);
    WSACleanup();
    printf("Server listen socket closed.\n");
}

void TcpSocketServer::closeConnection(SOCKET socket)
{
    if (socket != INVALID_SOCKET) {
        // shutdown the connection since we're done
        printBuffer("TcpSocketServer: Closing socket %d\n", socket);
        int iResult = shutdown(socket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
        }

        closesocket(socket);
        _client_socket = INVALID_SOCKET;
    }

	_connection_handled = true;
}

void TcpSocketServer::printBuffer(const char* format, ...)
{
    _mutex.lock();
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
    //puts(buffer);
    _mutex.unlock();
}

void TcpSocketServer::handleConnection(SOCKET socket)
{
    char buffer[DEFAULT_BUFLEN];
    _connection_handled = false;
    //_client_socket = socket;
	int iResult;

    //_timeout.attach(callback(this, &TcpSocketServer::closeConnection), 1);
    printBuffer("socket: %d\n", socket);

	// Receive until the peer shuts down the connection
	do {
        memset(buffer, '\0', DEFAULT_BUFLEN);
		iResult = recv(socket, buffer, DEFAULT_BUFLEN, 0);
		printBuffer("iResult: %d\n", iResult);
		if (iResult > 0) {
#if 0
            char* rp = _buffer;

            while(*rp != '\n') {
                putchar(*rp++);
            }
            putchar('\n');
            rp = strstr(_buffer, "\r\n\r\n");
            rp += 4;
            while(*rp != '\0') {
                putchar(*rp++);
            }
#endif
			printBuffer(buffer);
			/*for (unsigned int i = 0; i < strlen(_buffer); ++i) {
                printf("%02X ", _buffer[i]);
                //printf("%c", _buffer[i]);
			}*/
			break;
		}
		else if (iResult == 0) {
            printBuffer("No more stuff to receive..\n");
			//printf("Connection closing...\n");
			//sprintf(_buffer, "0,ERROR\r\n");
			closeConnection(socket);
			return;
			break;
		}
		else {
			printBuffer("recv failed with error: %d\n", WSAGetLastError());
			//sprintf(_buffer, "0,ERROR\r\n");
			//closeConnection();
			break;
		}

	} while (iResult > 0);

#if 1
    _timeout.detach();
    //if (iResult > 0) {

    HttpResponse response(socket);

    /*if (strstr(buffer, "GET /script.js") != NULL) {
        if (strstr(buffer, "If-None-Match: \"1\"") != NULL) {
            response.sendResponseNotModified("1");
        }
        else {
            response.sendResponseOk("text/javascript", "1");
            response.sendFile("/local/script.js");
        }
    }
    else*/
    if (strstr(buffer, "GET /style.css") != NULL) {
        if (strstr(buffer, "If-None-Match: \"1\"") != NULL) {
            response.sendResponseNotModified("1");
        }
        else {
            response.sendResponseOk("text/css", "1");
            response.sendFile("/local/style.css");
        }
    }
    else if (strstr(buffer, "GET /background.png") != NULL) {
        if (strstr(buffer, "If-None-Match: \"1\"") != NULL) {
            response.sendResponseNotModified("1");
        }
        else {
            response.sendResponseOk("image/png", "1");
            response.sendFile("/local/background.png");
        }
    }
    else if (strstr(buffer, "GET /speaker.png") != NULL) {
        if (strstr(buffer, "If-None-Match: \"1\"") != NULL) {
            response.sendResponseNotModified("1");
        }
        else {
            response.sendResponseOk("image/png", "1");
            response.sendFile("/local/speaker.png");
        }
    }
    else if (strstr(buffer, "GET /mute.png") != NULL) {
        if (strstr(buffer, "If-None-Match: \"1\"") != NULL) {
            response.sendResponseNotModified("1");
        }
        else {
            response.sendResponseOk("image/png", "1");
            response.sendFile("/local/mute.png");
        }
    }
    else if (strstr(buffer, "GET /settings.png") != NULL) {
        if (strstr(buffer, "If-None-Match: \"1\"") != NULL) {
            response.sendResponseNotModified("1");
        }
        else {
            response.sendResponseOk("image/png", "1");
            response.sendFile("/local/settings.png");
        }
    }
    else {
        int volume = 26;
        static int currentVolume = 26;
        static int input = 3;
        static bool muted = false;

        if (strstr(buffer, "POST /") != NULL) {
            bool validInput = false;

            // Parse input data from POST request
            HttpServer::parseCharValue(buffer, "pot=", &volume);
            HttpServer::parseCharValue(buffer, "spdif=", &input);
            switch (input) {
            case 0: case 1: case 2: case 3:
                validInput = true;
                break;
            default:
                input = 3;
                break;
            }

            if (volume == 0) {
                muted = !muted;
                if (!muted) {
                    volume = currentVolume;
                }
            } else {
                muted = false;
                currentVolume = volume;
            }
        }

        response.sendResponseOk("text/html");
        response.sendMainPage(input, volume);
    }
#else
	_timeout.detach();
	onDataReceived();
	while (!_connection_handled) {
        wait_ms(10);
	}
#endif
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

    _is_running = true;
    while (_is_running) {
        SOCKET socket = INVALID_SOCKET;
        // Accept a Connection
        while (_is_running && socket == INVALID_SOCKET){
            socket = accept(_listen_socket, NULL, NULL);
            if (socket < 0) {
                printf("Socket %d accepted\n", socket);
            }
        }

        if (_is_running) {
            std::thread t1(&TcpSocketServer::handleConnection, this, socket);
            //handleConnection(socket);
            t1.detach();
        }
    }
    // No longer need server socket
    closesocket(_listen_socket);
    WSACleanup();
    printf("TCP server stopped.\n");

    return 0;
}

size_t TcpSocketServer::getGmtDateTime(char* buff, int len)
{
    time_t rawtime;
    struct tm *info;

    time(&rawtime);
    /* Get GMT time */
    info = gmtime(&rawtime);

    // Date: Wed, 21 Oct 2015 07:28:00 GMT
    return strftime(buff, len, "%a, %d %b %Y %H:%M:%S GMT", info);
}

TcpSocketServer* TcpSocketServer::instance()
{
    static TcpSocketServer inst;
    return &inst;
}
