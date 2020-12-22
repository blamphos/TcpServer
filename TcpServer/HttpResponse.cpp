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

void HttpResponse::sendIndexPage(int volume, int input)
{
    memset(_buffer, '\0', DEFAULT_BUFLEN);
    _wp = _buffer;

    _wp += sprintf(_wp, "<!DOCTYPE HTML>\r\n");
    _wp += sprintf(_wp, "<html>\r\n");
        _wp += sprintf(_wp, "<head>\r\n");
            _wp += sprintf(_wp, "<link rel=\"icon\" href=\"data:,\">\r\n");
            _wp += sprintf(_wp, "<link rel=\"stylesheet\" href=\"style.css\">\r\n");
            _wp += sprintf(_wp, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\" charset=\"UTF-8\">\r\n");
            _wp += sprintf(_wp, "<title>GEVOL 3.0</title>\r\n");
        _wp += sprintf(_wp, "</head>\r\n");

        _wp += sprintf(_wp, "<body>\r\n");
            _wp += sprintf(_wp, "<div class=\"tools\">\r\n");
                _wp += sprintf(_wp, "<a href=\"settings.html\">\r\n");
                _wp += sprintf(_wp, "<img class=\"settings\" src=\"settings.png\" alt=\"Settings\"/></a>\r\n");
            _wp += sprintf(_wp, "</div>\r\n");
                _wp += sprintf(_wp, "<div class=\"controls\">\r\n");
                    _wp += sprintf(_wp, "<div>\r\n");
                    _wp += sprintf(_wp, "<button id=\"coax1\" class=\"btn1 btn%d\" onclick=\"onInputChanged(0)\">COAX1</button>\r\n", input == 0 ? 1 : 2);
                    _wp += sprintf(_wp, "<button id=\"coax2\" class=\"btn1 btn%d\" onclick=\"onInputChanged(1)\">COAX2</button>\r\n", input == 1 ? 1 : 2);
                    _wp += sprintf(_wp, "<br>\r\n");
                    _wp += sprintf(_wp, "<button id=\"opt1\" class=\"btn1 btn%d\" onclick=\"onInputChanged(2)\">OPT1</button>\r\n", input == 2 ? 1 : 2);
                    _wp += sprintf(_wp, "<button id=\"auto\" class=\"btn1 btn%d\" onclick=\"onInputChanged(3)\">AUTO</button>\r\n", input == 3 ? 1 : 2);
                _wp += sprintf(_wp, "</div>\r\n");
                _wp += sprintf(_wp, "<div class=\"img-container\">\r\n");
                    _wp += sprintf(_wp, "<img id=\"audio\" src=\"speaker.png\" alt=\"sp\" class=\"audio%d\"/>\r\n", volume == 0 ? 2 : 1);
                    _wp += sprintf(_wp, "<img id=\"mute\" src=\"mute.png\" alt=\"mute\" class=\"audio%d\"/>\r\n", volume == 0 ? 1 : 2);
                _wp += sprintf(_wp, "</div>\r\n");
                _wp += sprintf(_wp, "<input type=\"range\" min=\"0\" max=\"99\" value=\"%d\" class=\"slider\" id=\"myRange\">\r\n", volume);
            _wp += sprintf(_wp, "</div>\r\n");
            _wp += sprintf(_wp, "<script src=\"script.js\"></script>\r\n");
        _wp += sprintf(_wp, "</body>\r\n");
    _wp += sprintf(_wp, "</html>\r\n");
  
    //puts(_buffer);
    _server->sendBuffer(_buffer, _wp - _buffer);
}
