#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

class HttpServer;
class HttpResourceFile;

class HttpResponse
{
public:
    HttpResponse(HttpServer* server);
    void send(HttpResourceFile* resource);
    void sendResponseOk();
    void sendResponseOk(HttpResourceFile* resource);
    void sendResponseNotFound();
    void sendIndexPage(int volume, int input);
    bool sendFile(HttpResourceFile* resource);
    virtual ~HttpResponse();

private:
    enum ConstantsT {
        DEFAULT_BUFLEN = 4096,
    };

    int getGmtDateTime(char* buff);

    char _buffer[DEFAULT_BUFLEN];
    char* _wp;
    HttpServer* _server;
};

#endif
