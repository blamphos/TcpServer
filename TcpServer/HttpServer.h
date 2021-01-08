#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "TcpSocketServer.h"
#include "EmbeddedMessages.h"
#include "SafeQueue.h"

class HttpResponse;
class HttpResourceFile;

class HttpServer : public TcpSocketServer {
public:
    HttpServer();
    ~HttpServer();
	void sendResponse(HttpResponseTypeT type, int level, int input, bool autofind, uint32_t spdifData);
	bool sendBuffer(char* buffer, int len);

private:
	enum ConstantsT {
		DEFAULT_PORT = 80,
		DEFAULT_BUFLEN = 2048,
	};

    enum HttpRequestTypeT {
        NotDefined = 0,
        Get,
        Post
    };

	void handleConnection(SOCKET socket);
	void closeSocket();
	void parseCharValue(char* buff, const char* tag, int* value);

	char _buffer[DEFAULT_BUFLEN];
	HttpResponse* _response;
	HttpRequestTypeT _requestType;
	HttpResourceFile* _resourceIndexHtml;
	HttpResourceFile* _resourceStyleCss;
	HttpResourceFile* _resourceRsMinStyleCss;
	HttpResourceFile* _resourceRsMinScriptJs;
	HttpResourceFile* _resourceJQueryMinScriptJs;
	HttpResourceFile* _resourceBackgroundJpg;
	HttpResourceFile* _resourceFaviconPng;
	HttpResourceFile* _resourceInputHtml;

	SOCKET _socket;
	SafeQueue<int> _sockets;
};

#endif
