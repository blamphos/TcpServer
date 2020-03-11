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

void HttpResponse::sendMainPage(int input, int volume)
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
    _wp += sprintf(_wp, "<a href=\"settings.html\">\r\n");
    _wp += sprintf(_wp, "<img class=\"settings\" src=\"settings.png\" alt=\"Settings\"/></a>\r\n");

    _wp += sprintf(_wp, "<header><h2>GEVOL 3.0</h2></header>\r\n");

    _wp += sprintf(_wp, "<article>\r\n");
    _wp += sprintf(_wp, "<FORM action=\"/\" method=\"post\" id=\"form\">\r\n");
    _wp += sprintf(_wp, "<div class=\"inputs\">\r\n");
    _wp += sprintf(_wp, "<br><br><br><br><br><br><br><br>\r\n");
    _wp += sprintf(_wp, "<button name=\"spdif\" id=\"coax1\" class=\"btn1 btn%d\" value=\"0\">COAX1</button>\r\n", input == 0 ? 1 : 2);
    _wp += sprintf(_wp, "<button name=\"spdif\" id=\"coax2\" class=\"btn1 btn%d\" value=\"1\">COAX2</button>\r\n", input == 1 ? 1 : 2);
    _wp += sprintf(_wp, "<br>\r\n");
    _wp += sprintf(_wp, "<button name=\"spdif\" id=\"opt1\" class=\"btn1 btn%d\" value=\"2\">OPT1</button>\r\n", input == 2 ? 1 : 2);
    _wp += sprintf(_wp, "<button name=\"spdif\" id=\"auto\" class=\"btn1 btn%d\" value=\"3\">AUTO</button>\r\n", input == 3 ? 1 : 2);
    _wp += sprintf(_wp, "</div>\r\n");
    _wp += sprintf(_wp, "<div class=\"slidecontainer\">\r\n");
    _wp += sprintf(_wp, "<img id=\"audio\" src=\"speaker.png\" alt=\"sp\" class=\"audio1 audio%d\" ontouchend='mute()'/>\r\n", volume == 0 ? 2 : 1);
    _wp += sprintf(_wp, "<img id=\"mute\" src=\"mute.png\" alt=\"mute\" class=\"audio1 audio%d\" ontouchend='mute()'/>\r\n", volume == 0 ? 1 : 2);
    _wp += sprintf(_wp, "<input type=\"range\" name=\"pot\" min=\"0\" max=\"99\" value=\"%d\" class=\"slider\" id=\"myRange\">\r\n", volume);
    _wp += sprintf(_wp, "</div>\r\n");

    _wp += sprintf(_wp, "</FORM>\r\n");

    //_wp += sprintf(_wp, "<script src=\"script.js\"></script>\r\n");
    _wp += sprintf(_wp, "<script>\r\n");
    _wp += sprintf(_wp, "var mainForm = document.getElementById(\"form\");\r\n");
    _wp += sprintf(_wp, "var slider = document.getElementById(\"myRange\");\r\n");
    _wp += sprintf(_wp, "slider.onchange = function() {mainForm.submit();}\r\n");
    _wp += sprintf(_wp, "var audio_icon = document.getElementById(\"audio\");\r\n");
    _wp += sprintf(_wp, "var mute_icon = document.getElementById(\"mute\");\r\n");
    _wp += sprintf(_wp, "function toggleMute() {slider.value = 0; mainForm.submit();}\r\n");
    _wp += sprintf(_wp, "audio_icon.addEventListener(\"click\", toggleMute, false);\r\n");
    _wp += sprintf(_wp, "mute_icon.addEventListener(\"click\", toggleMute, false);\r\n");
    _wp += sprintf(_wp, "</script>\r\n");

    _wp += sprintf(_wp, "</article>\r\n");
    _wp += sprintf(_wp, "</body>\r\n");
    _wp += sprintf(_wp, "</html>\r\n");

    //printBuffer(_buffer);
    sendBuffer(_wp - _buffer);
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
