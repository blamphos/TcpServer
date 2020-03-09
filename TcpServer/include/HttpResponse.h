#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <ws2tcpip.h>

class HttpResponse
{
public:
    enum StatusCodeT {
        OK,
        NotModified,
    };

    HttpResponse(SOCKET socket);
    void addHeaders(StatusCodeT status, const char* contentType, int maxAge, const char* eTag);
    bool sendFile(const char* file);
    virtual ~HttpResponse();

private:
    enum ConstantsT {
        DEFAULT_BUFLEN = 4096
    };

    void closeConnection();
	size_t getGmtDateTime(char* buff);
    bool sendBuffer(size_t len);
    static void printBuffer(const char* format, ...);

    SOCKET _socket;
    char _buffer[DEFAULT_BUFLEN];
    char* _wp;
    static std::mutex _mutex;
};

#endif
