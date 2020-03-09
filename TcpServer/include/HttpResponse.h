#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <ws2tcpip.h>

class HttpResponse
{
public:
    enum StatusCodeT {
        OK,
        NotModified,
        NotFound
    };

    HttpResponse(SOCKET socket);
    void addHeaders(StatusCodeT status, const char* contentType, int maxAge, const char* eTag);
	size_t getGmtDateTime(char* buff);
    bool sendFile(const char* file);
    virtual ~HttpResponse();

private:
    enum ConstantsT {
        DEFAULT_BUFLEN = 4096
    };

    void closeConnection();
    bool sendBuffer(int len);

    SOCKET _socket;
    char _buffer[DEFAULT_BUFLEN];
    char* _wp;
    size_t _buflen;
};

#endif
