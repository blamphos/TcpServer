#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <ws2tcpip.h>

class HttpResponse
{
public:
    HttpResponse(SOCKET socket);
    void sendResponseOk(const char* contentType, const char* eTag = NULL);
    void sendResponseNotModified(const char* eTag);
    void sendResponseNotFound();
    void sendMainPage(int input, int volume);
    bool sendFile(const char* file);
    virtual ~HttpResponse();

private:
    enum ConstantsT {
        DEFAULT_BUFLEN = 4096,
        MAX_AGE = 86400,
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
