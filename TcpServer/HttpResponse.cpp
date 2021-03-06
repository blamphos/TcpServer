#include <time.h>
#include <iostream>
#include <cstring>
#include "HttpResponse.h"
#include "HttpServer.h"
#include "HttpResourceFile.h"

HttpResponse::HttpResponse(HttpServer* server) :  
    _wp(_buffer),
    _server(server)
{

}

HttpResponse::~HttpResponse()
{
    
}

void HttpResponse::send(HttpResourceFile* resource)
{
    //printf("GET: %s\n", resource->getFileName());
    sendResponseOk(resource);
    sendFile(resource);
}

void HttpResponse::sendResponseOk()
{
    memset(_buffer, '\0', DEFAULT_BUFLEN);
    _wp = _buffer;
    _wp += sprintf(_buffer, "HTTP/1.1 200 OK\r\n");
    _wp += sprintf(_wp, "Content-Type: %s\r\n", HttpResourceFile::convertTypeToStr(HttpResourceFile::TEXT_HTML));
    _wp += sprintf(_wp, "\r\n");
    
    _server->sendBuffer(_buffer, _wp - _buffer);
}

void HttpResponse::sendResponseOk(HttpResourceFile* resource)
{
    memset(_buffer, '\0', DEFAULT_BUFLEN);
    _wp = _buffer;
    _wp += sprintf(_buffer, "HTTP/1.1 200 OK\r\n");
    _wp += sprintf(_wp, "Content-Type: %s\r\n", resource->getTypeStr());
    _wp += sprintf(_wp, "Date: ");
    _wp += getGmtDateTime(_wp);
    _wp += sprintf(_wp, "\r\n");
    _wp += sprintf(_wp, "Cache-Control: private\r\n");
    _wp += sprintf(_wp, "Cache-Control: max-age=0\r\n");
    _wp += sprintf(_wp, "Content-Length: %d\r\n", resource->getFileSize());
    _wp += sprintf(_wp, "\r\n");
    
    _server->sendBuffer(_buffer, _wp - _buffer);
}

void HttpResponse::sendResponseNotFound()
{
    memset(_buffer, '\0', DEFAULT_BUFLEN);
    _wp = _buffer;
    _wp += sprintf(_buffer, "HTTP/1.1 404 Not Found\r\n");
    
    _server->sendBuffer(_buffer, _wp - _buffer);
}

int HttpResponse::getGmtDateTime(char* buff)
{
    time_t rawtime;
    struct tm* info;

    time(&rawtime);
    info = gmtime(&rawtime);

    return strftime(buff, 30, "%a, %d %b %Y %H:%M:%S GMT", info);
}

bool HttpResponse::sendFile(HttpResourceFile* resource)
{
    size_t buflen = 0;
    bool success = true;

    FILE* fp = fopen(resource->getFileName(), "rb");
    if (fp == NULL) {
        return false;
    }

    while (1) {
        buflen = fread(_buffer, 1, DEFAULT_BUFLEN, fp);
        if (buflen < 1) {
            if (!feof(fp)) {
                puts("error while reading the file...");
                success = false;
            }
            break;
        }

        if (!_server->sendBuffer(_buffer, buflen)) {
            success = false;
            break;
        }
    }

    return success;
}
