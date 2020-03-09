#include <time.h>
#include <stdio.h>
#include <string.h>
#include <mutex>
#include "HttpResponse.h"

std::mutex HttpResponse::_mutex;

HttpResponse::HttpResponse(SOCKET socket) : _socket(socket), _wp(_buffer)
{
    memset(_buffer, '\0', DEFAULT_BUFLEN);
}

HttpResponse::~HttpResponse()
{
    closeConnection();
}

void HttpResponse::sendResponseOk(const char* contentType, const char* eTag)
{
    _wp += sprintf(_buffer, "HTTP/1.1 200 OK\r\n");
    if (contentType != NULL) {
        _wp += sprintf(_wp, "Content-Type: %s\r\n", contentType);
    }

    if (eTag != NULL) {
        _wp += sprintf(_wp, "Date: ");
        _wp += getGmtDateTime(_wp);
        _wp += sprintf(_wp, "\r\n");
        _wp += sprintf(_wp, "ETag: \"%s\"\r\n", eTag);
        _wp += sprintf(_wp, "Cache-Control: public\r\n");
        _wp += sprintf(_wp, "Cache-Control: max-age=%d\r\n", MAX_AGE);
    }
    _wp += sprintf(_wp, "\r\n");

    printBuffer(_buffer);
    sendBuffer(_wp - _buffer);
}

void HttpResponse::sendResponseNotModified(const char* eTag)
{
    _wp += sprintf(_buffer, "HTTP/1.1 304 Not Modified\r\n");
    if (eTag != NULL) {
        _wp += sprintf(_wp, "ETag: \"%s\"\r\n", eTag);
    }
    _wp += sprintf(_wp, "Cache-Control: public\r\n");
    _wp += sprintf(_wp, "Cache-Control: max-age=%d\r\n", MAX_AGE);
    _wp += sprintf(_wp, "\r\n");

    printBuffer(_buffer);
    sendBuffer(_wp - _buffer);
}

void HttpResponse::sendResponseNotFound()
{
    _wp += sprintf(_buffer, "HTTP/1.1 404 Not Found\r\n");
    printBuffer(_buffer);
    sendBuffer(_wp - _buffer);
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
    size_t buflen = 0;
    bool success = true;

    FILE* fp = fopen(file, "rb");
    if (fp == NULL) {
        return false;
    }

    while (1) {
        buflen = fread(_buffer, 1, DEFAULT_BUFLEN, fp);
        if (buflen < 1) {
            if (!feof(fp)) {
                printBuffer("error while reading the file...");
                success = false;
            }
            break;
        }

        if (!sendBuffer(buflen)) {
            success = false;
            break;
        }
    }

    return success;
}

void HttpResponse::closeConnection()
{
    if (_socket != INVALID_SOCKET) {
        // shutdown the connection since we're done
        printBuffer("Closing socket %d\n", _socket);
        if (shutdown(_socket, SD_SEND) == SOCKET_ERROR) {
            printBuffer("shutdown failed with error: %d\n", WSAGetLastError());
        }

        closesocket(_socket);
    }
}

void HttpResponse::printBuffer(const char* format, ...)
{
    _mutex.lock();

    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);

    _mutex.unlock();
}

bool HttpResponse::sendBuffer(size_t len)
{
    bool success = true;

    if (send(_socket, _buffer, len, 0) == SOCKET_ERROR) {
        printBuffer("send failed with error: %d\n", WSAGetLastError());
        success = false;
    }

    return success;
}
