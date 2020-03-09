#ifndef ESP8266_SIMULATED_H
#define ESP8266_SIMULATED_H

#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501

#include <ws2tcpip.h>
#include "mbed.h"
#include <mutex>

#define DEFAULT_PORT	"80"

class TcpSocketServer
{
public:
    void attach(Callback<void()> cb);
    void detach();
    void readBuffer(char* buff, int* len);
    void start();
    void stop();
    void closeConnection(SOCKET socket);
    void sendBuffer(const char* buff, int len);
	static TcpSocketServer* instance();

private:
    enum ConstantsT {
        DEFAULT_BUFLEN = 4096
    };

	TcpSocketServer();
	void handleConnection(SOCKET socket);
	int serverThreadImp();
	size_t getGmtDateTime(char* buff, int len);
	Callback<void()> onDataReceived;
	static void printBuffer(const char* format, ...);
    static bool sendFile(const char* path, char* buff, int socket);

	SOCKET _listen_socket;
    SOCKET _client_socket;
	//char _buffer[DEFAULT_BUFLEN];
	bool _is_running;
	bool _connection_handled;
	Timeout _timeout;
	static std::mutex _mutex;
};

#endif
