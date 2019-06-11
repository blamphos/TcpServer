#ifndef ESP8266_SIMULATED_H
#define ESP8266_SIMULATED_H

#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501

#include <ws2tcpip.h>
#include <thread>
#include "mbed.h"

#define DEFAULT_PORT	"80"

class ESP8266Simulated
{
public:
    void attach(Callback<void()> cb);
    void detach();
    void readBuffer(char* buff, int* len);
    void start();
    void stop();
    void sendBuffer(const char* buff, int len);
	static ESP8266Simulated* instance();

private:
    enum ConstantsT {
        DEFAULT_BUFLEN = 4096
    };

	ESP8266Simulated();
	void handleConnection();
	int serverThreadImp();
	Callback<void()> onDataReceived;

	SOCKET _listen_socket;
    SOCKET _client_socket;
	std::thread* _serverThread;
	char _buffer[DEFAULT_BUFLEN];
};

#endif
