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
	void sendResponse(HttpResponseTypeT type, int level, int input);
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

	void threadFunc(SOCKET p_socket);
	void handleConnection(SOCKET socket);
	void closeSocket();
	void parseCharValue(char* buff, const char* tag, int* value);

	char _buffer[DEFAULT_BUFLEN];
	HttpResponse* _response;
	HttpRequestTypeT _requestType;
	HttpResourceFile* _resourceIndexHtml;
	HttpResourceFile* _resourceScriptJs;
	HttpResourceFile* _resourceStyleCss;
	HttpResourceFile* _resourceBackgroundJpg;
	HttpResourceFile* _resourceSpeakerPng;
	HttpResourceFile* _resourceMutePng;
	//HttpResourceFile* _resourceSettingsPng;
	SOCKET _socket;
	SafeQueue<int> _sockets;
};

#endif