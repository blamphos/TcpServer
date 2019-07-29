#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "EventQueue.h"

class TCPConnection;

class HttpServer {
public:
    HttpServer();
    ~HttpServer();
    void handleRequest();
	void handleMessage(message_t msg);

private:
    enum HttpRequestTypeT {
        NotDefined = 0,
        Get,
        Post
    };

    void sendResponse();
	void parseCharValue(char* buff, const char* tag, int* value);
	void setVolumeLevel(char* buff, int volume);
	void setButtonState(char* buff, const char* tag, bool enabled);

	TCPConnection* _tcp;
    HttpRequestTypeT _requestType;
};

#endif
