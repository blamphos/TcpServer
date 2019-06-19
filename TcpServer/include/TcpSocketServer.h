#ifndef ESP8266_SIMULATED_H
#define ESP8266_SIMULATED_H

#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501

#include <ws2tcpip.h>
#include "mbed.h"

#define DEFAULT_PORT	"80"

class TcpSocketServer
{
public:
    void attach(Callback<void()> cb);
    void detach();
    void readBuffer(char* buff, int* len);
    void start();
    void stop();
    void closeConnection();
    void sendBuffer(const char* buff, int len);
	static TcpSocketServer* instance();

private:
    enum ConstantsT {
        DEFAULT_BUFLEN = 4096
    };

	TcpSocketServer();
	void handleConnection(SOCKET socket);
	int serverThreadImp();
	Callback<void()> onDataReceived;

	SOCKET _listen_socket;
    SOCKET _client_socket;
	char _buffer[DEFAULT_BUFLEN];
	bool _is_running;
	bool _connection_handled;
};

#endif
