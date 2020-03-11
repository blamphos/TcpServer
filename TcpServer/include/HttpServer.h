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
	static void parseCharValue(char* buff, const char* tag, int* value);
	static void setVolumeLevel(char* buff, int volume);

private:
    enum HttpRequestTypeT {
        NotDefined = 0,
        Get,
        Post
    };

    void sendResponse();
	void setButtonState(char* buff, const char* tag, bool enabled);

	TCPConnection* _tcp;
    HttpRequestTypeT _requestType;
};

#endif
