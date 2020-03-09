#include <time.h>
#include <stdio.h>
#include <string.h>
#include "HttpResponse.h"

HttpResponse::HttpResponse(SOCKET socket) : _socket(socket)
{
    memset(_buffer, '\0', DEFAULT_BUFLEN);
}

HttpResponse::~HttpResponse()
{

}

void HttpResponse::addHeaders(StatusCodeT status, const char* contentType, int maxAge, const char* eTag)
{
    _wp = _buffer;

    switch (status) {
    case StatusCodeT::OK:
        _wp += sprintf(_buffer, "HTTP/1.1 200 OK\r\n");
        if (contentType != NULL) {
            _wp += sprintf(_wp, "Content-Type: %s\r\n", contentType);
        }
        _wp += sprintf(_wp, "Date: ");
        _wp += getGmtDateTime(_wp);

        break;
    case StatusCodeT::NotModified:
        _wp += sprintf(_buffer, "HTTP/1.1 304 Not Modified\r\n");
        if (eTag != NULL) {
            _wp += sprintf(_wp, "ETag: \"%s\"\r\n", eTag);
        }
        _wp += sprintf(_wp, "Cache-Control: public\r\n");
        _wp += sprintf(_wp, "Cache-Control: max-age=%d\r\n\r\n", maxAge);

        break;
    default:
        _wp += sprintf(_buffer, "HTTP/1.1 404 Not Found\r\n");
        break;
    }

    //sendBuffer(_wp - _buffer);
}

size_t HttpResponse::getGmtDateTime(char* buff)
{
    time_t rawtime;
    struct tm *info;

    time(&rawtime);
    info = gmtime(&rawtime);

    return strftime(buff, 30, "%a, %d %b %Y %H:%M:%S GMT", info);
}

bool HttpResponse::sendFile(const char* file)
{
    bool success = true;

    return success;
}

void HttpResponse::closeConnection()
{
    if (_socket != INVALID_SOCKET) {
        // shutdown the connection since we're done
        //printBuffer("Closing socket %d\n", socket);
        if (shutdown(_socket, SD_SEND) == SOCKET_ERROR) {
            printf("shutdown failed with error: %d\n", WSAGetLastError());
        }

        closesocket(_socket);
        _socket = INVALID_SOCKET;
    }
}

bool HttpResponse::sendBuffer(int len)
{
    bool success = true;

    if (send(_socket, _buffer, len, 0) == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        success = false;
    }

    return success;
}
