#ifndef ESP8266_SIMULATED_H
#define ESP8266_SIMULATED_H

#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x501

#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>

#define DEFAULT_BUFLEN	128
#define DEFAULT_PORT	"80"

class ESP8266Simulated
{
public:
    void start();
    void stop();
	static ESP8266Simulated* instance();

private:
    enum HttpRequestTypeT {
        NotDefined = 0,
        Get,
        Post
    };

	ESP8266Simulated();
	void parseCharValue(char* buff, const char* tag, int* value);
	void handleConnection(SOCKET ClientSocket);
	void setVolumeLevel(char* buff, int volume);
	void setButtonState(char* buff, bool enabled);
	int serverThreadImp();

	SOCKET _listenSocket;
	std::thread* _serverThread;
};

#endif
