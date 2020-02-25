#include <thread>
#include <time.h>
#include "TcpSocketServer.h"

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
#endif // 0
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

void TcpSocketServer::closeConnection(int socket)
{
    if (socket != INVALID_SOCKET) {
        // shutdown the connection since we're done
        printf("Closing socket %d\n", socket);
        int iResult = shutdown(socket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
        }

        closesocket(socket);
        _client_socket = INVALID_SOCKET;
    }

	_connection_handled = true;
}

void TcpSocketServer::handleConnection(SOCKET socket)
{
    char buffer[DEFAULT_BUFLEN];
    _connection_handled = false;
    //_client_socket = socket;
	int iResult;

    //_timeout.attach(callback(this, &TcpSocketServer::closeConnection), 1);
    printf("socket: %d\n", socket);

	// Receive until the peer shuts down the connection
	do {
        memset(buffer, '\0', DEFAULT_BUFLEN);
		iResult = recv(socket, buffer, DEFAULT_BUFLEN, 0);
		printf("iResult: %d\n", iResult);
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
			printf(buffer);
			/*for (unsigned int i = 0; i < strlen(_buffer); ++i) {
                printf("%02X ", _buffer[i]);
                //printf("%c", _buffer[i]);
			}*/
			break;
		}
		else if (iResult == 0) {
            printf("No more stuff to receive..\n");
			//printf("Connection closing...\n");
			//sprintf(_buffer, "0,ERROR\r\n");
			closeConnection(socket);
			return;
			break;
		}
		else {
			printf("recv failed with error: %d\n", WSAGetLastError());
			//sprintf(_buffer, "0,ERROR\r\n");
			//closeConnection();
			break;
		}

	} while (iResult > 0);

#if 1
    _timeout.detach();
    //if (iResult > 0) {
    if (1) {
        FILE* fp = NULL;
        char buff[DEFAULT_BUFLEN] = {0};
        int n = sprintf(buff, "HTTP/1.1 200 OK\r\n");
        char* wp = buff + n;
        size_t buflen = 0;

        if (strstr(buffer, "GET /script.js") != NULL) {
            wp += sprintf(wp, "Content-Type: text/javascript\r\n");
            wp += sprintf(wp, "Cache-Control: max-age=86400\r\n");
            wp += sprintf(wp, "ETag: \"1\"\r\n\r\n");
            //printf(buff);
            buflen = wp - buff;
            int iSendResult = send(socket, buff, buflen, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
            }

            fp = fopen("/local/script.js", "rb");
            while (1) {
                buflen = fread(buff, 1, sizeof(buff), fp);
                if (buflen < 1) {
                    if (!feof(fp)) {
                        // a read error occured...
                    }
                    break;
                }
                int iSendResult = send(socket, buff, buflen, 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    //closesocket(ClientSocket);
                    //WSACleanup();
                    //break;
                }
            }
            closeConnection(socket);
        }
        else if (strstr(buffer, "GET /style.css") != NULL) {
            wp += sprintf(wp, "Content-Type: text/css\r\n");
            wp += sprintf(wp, "Cache-Control: max-age=86400\r\n\r\n");

            buflen = wp - buff;
            int iSendResult = send(socket, buff, buflen, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
            }

            fp = fopen("/local/style.css", "rb");
            while (1) {
                buflen = fread(buff, 1, sizeof(buff), fp);
                if (buflen < 1) {
                    if (!feof(fp)) {
                        // a read error occured...
                    }
                    break;
                }
                int iSendResult = send(socket, buff, buflen, 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    //closesocket(ClientSocket);
                    //WSACleanup();
                    //break;
                }
            }
            closeConnection(socket);
        }
        else if (strstr(buffer, "GET /background.png") != NULL) {
            wp += sprintf(wp, "Accept-Ranges: bytes\r\n");
            //wp += sprintf(wp, "Connection: Keep-Alive\r\n");
            wp += sprintf(wp, "Content-Length: 110226\r\n");
            wp += sprintf(wp, "Content-Type: image/png\r\n");
            //wp += sprintf(wp, "Date: Mon, 24 Feb 2020 17:43:22 GMT\r\n");
            wp += sprintf(wp, "Date: ");
            wp += getGmtDateTime(wp, 80);
            wp += sprintf(wp, "\r\nETag: \"123\"\r\n");
            //wp += sprintf(wp, "Last-Modified: Mon, 24 Feb 2020 17:10:37 GMT\r\n");
            wp += sprintf(wp, "Server: Gevol 3.0\r\n\r\n");
            printf(buff);
            buflen = wp - buff;
            int iSendResult = send(socket, buff, buflen, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
            }

            fp = fopen("/local/background.png", "rb");
            while (1) {
                buflen = fread(buff, 1, sizeof(buff), fp);
                if (buflen < 1) {
                    if (!feof(fp)) {
                        // a read error occured...
                    }
                    break;
                }
                int iSendResult = send(socket, buff, buflen, 0);
                if (iSendResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    //closesocket(ClientSocket);
                    //WSACleanup();
                    //break;
                }
            }
            closeConnection(socket);
        }
        else {
            fp = fopen("/local/index.html", "rb");
            wp += sprintf(wp, "Content-Type: text/html\r\n\r\n");
            char c = 0;
            do {
                c = fgetc(fp);
                if (feof(fp)) {
                    break;
                }
                *wp++ = c;
            } while (1);
            /*while (!feof(fp)) {
                *wp++ = fgetc(fp);
            }*/
            fclose(fp);

            //printf(buff);
            //sendBuffer(buff, strlen(buff));
            int iSendResult = send(socket, buff, strlen(buff), 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                //closesocket(ClientSocket);
                //WSACleanup();
                //break;
            }
            closeConnection(socket);
        }

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
            //std::thread t1(&TcpSocketServer::handleConnection, this, socket);
            handleConnection(socket);
            //t1.join();
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
