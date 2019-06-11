#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

class HttpServer {
public:
    HttpServer();

    void handleRequest(char* buff, int len);
	void parseCharValue(char* buff, const char* tag, int* value);
	void setVolumeLevel(char* buff, int volume);
	void setButtonState(char* buff, bool enabled);
    void sendResponse();
private:
    enum HttpRequestTypeT {
        NotDefined = 0,
        Get,
        Post
    };

    enum ConstantsT {
        BUFFER_LEN = 2048
    };

    HttpRequestTypeT _requestType;
};

#endif
