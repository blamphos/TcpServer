#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "EventQueue.h"

class ESP8266;

class HttpServer {
public:
    HttpServer();
    void handleRequest();
    void sendResponse();
	void handleMessage(message_t msg);

private:
    enum HttpRequestTypeT {
        NotDefined = 0,
        Get,
        Post
    };

	void parseCharValue(char* buff, const char* tag, int* value);
	void setVolumeLevel(char* buff, int volume);
	void setButtonState(char* buff, bool enabled);

	ESP8266* _esp;
    HttpRequestTypeT _requestType;
};

#endif
