#include "HttpServer.h"
#include "HttpResponse.h"
#include "HttpResourceFile.h"
#include "EventQueue.h"

HttpServer::HttpServer() :
	TcpSocketServer(DEFAULT_PORT),
	_response(new HttpResponse(this)),
	_requestType(NotDefined),
	_socket(INVALID_SOCKET)
{
	_resourceIndexHtml = new HttpResourceFile("../home/pi/index.html", HttpResourceFile::TEXT_HTML);
	_resourceScriptJs = new HttpResourceFile("../home/pi/script.js", HttpResourceFile::TEXT_JAVASCRIPT);
	_resourceStyleCss = new HttpResourceFile("../home/pi/style.css", HttpResourceFile::TEXT_CSS);
	_resourceBackgroundJpg = new HttpResourceFile("../home/pi/background.jpg", HttpResourceFile::IMAGE_JPG);
	_resourceSpeakerPng = new HttpResourceFile("../home/pi/speaker.png", HttpResourceFile::IMAGE_PNG);
	_resourceMutePng = new HttpResourceFile("../home/pi/mute.png", HttpResourceFile::IMAGE_PNG);
	//_resourceSettingsPng = new HttpResourceFile("../home/pi/settings.png", HttpResourceFile::IMAGE_PNG);
}

HttpServer::~HttpServer()
{
	delete _response;
	delete _resourceIndexHtml;
	delete _resourceScriptJs;
	delete _resourceStyleCss;
	delete _resourceBackgroundJpg;
	delete _resourceSpeakerPng;
	delete _resourceMutePng;
	//delete _resourceSettingsPng;

	shutdown();
}

void HttpServer::handleConnection(SOCKET socket)
{
	int len = 0;
	int iResult = 0;

	memset(_buffer, '\0', DEFAULT_BUFLEN);
	char* wr = _buffer;

	if (_socket == INVALID_SOCKET) {
		_socket = socket;
	}
	else {
		puts("another socket already serving!");
		return;
	}
	printf("accepted connection from socket %d\n", _socket);
	puts("----------------------------");

	while (1) {
		if ((DEFAULT_BUFLEN - len) <= 0) {
			puts("buffer full");
			break;
		}

		iResult = recv(_socket, wr, DEFAULT_BUFLEN - len, 0);
		if (iResult > 0) {
			printf("recv: %d bytes from socket %d\n", iResult, _socket);
			len += iResult;
			wr += iResult;
			if ((wr - _buffer) >= DEFAULT_BUFLEN) {
				puts("buffer full, rounding over");
				wr = _buffer;
			}
			
			if (strstr(_buffer, "\r\n\r\n") == NULL) {
				continue;
			}

			if (strstr(_buffer, "GET") != NULL) {
				_requestType = Get;
				break;
			}
			else if (strstr(_buffer, "POST") != NULL) {
				_requestType = Post;
				break;
			}
		}
		else if (iResult == 0) {
			puts("no data available, client disconnects");
			break;
			//_response->sendResponseOk();
			//closeSocket();
			//return;
		}
		else {
			puts("error recv");
			closeSocket();
			return;
		}
	}
	
	if (_requestType == Post) {
		int volume = -1;
		int input = -1;
		int validInput = 0;
		
		// Parse input data from POST request
//		parseCharValue(_buffer, "pot=", &volume);
		parseCharValue(_buffer, "spdif=", &input);
		switch (input) {
		case 0: case 1: case 2: case 3:
			validInput = 1;
			break;
		default:
			input = 3;
			break;
		}
		
		uint32_t data = input | (validInput << 2) | (volume << 3);
		EventQueue::instance()->post(EVENT_HTTP_REQUEST_POST, data);
		int s = _sockets.dequeue();
	}
	else if (_requestType == Get) {
		if (strstr(_buffer, "GET / HTTP") != NULL) {
			//EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_GET);
			//int s =_sockets.dequeue();
			_response->sendResponseOk();
			_response->sendFile(_resourceIndexHtml);
			//_response->sendIndexPage(22, 3);
		}
		
		if (strstr(_buffer, "GET /style.css") != NULL) {
			_response->send(_resourceStyleCss);
		}
		else if (strstr(_buffer, "GET /script.js") != NULL) {
			_response->send(_resourceScriptJs);
		}
		else if (strstr(_buffer, "GET /background.jpg") != NULL) {
			_response->send(_resourceBackgroundJpg);
		}
		else if (strstr(_buffer, "GET /speaker.png") != NULL) {
			_response->send(_resourceSpeakerPng);
		}
		else if (strstr(_buffer, "GET /mute.png") != NULL) {
			_response->send(_resourceMutePng);
		}
		/*else if (strstr(_buffer, "GET /settings.png") != NULL) {
			_response->send(_resourceSettingsPng);
		}*/

		closeSocket();
	}
	else {
		if (strstr(_buffer, "shutdown") != NULL) {
			closeSocket();
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		puts("Unknown request");
		puts(_buffer);
		closeSocket();
	}
}

void HttpServer::sendResponse(HttpResponseTypeT type, int level, int input)
{
	if (_socket == INVALID_SOCKET) {
		puts("sendResponse: invalid socket");
		return;
	}

	int socket = _socket;
	printf("level: %d, input: %d\n", level, input);
	_response->sendResponseOk();

	if (type == HttpResponseTypeT::HTTP_RESPONSE_GET) {
		//_response->sendIndexPage(level, input);
		//_response->send(_resourceIndexHtml);
		_response->sendFile(_resourceIndexHtml);
	}
	else if (type == HttpResponseTypeT::HTTP_RESPONSE_POST) {
		_response->sendResponseOk();

		char buffer[32] = { '\0' };
		sprintf(buffer, "%d;%d", level, input);
		sendBuffer(buffer, strlen(buffer));
	}

	closeSocket();

	_sockets.enqueue(socket);
}

bool HttpServer::sendBuffer(char* buffer, int len)
{
	bool success = true;

	if (_socket == INVALID_SOCKET || send(_socket, buffer, len, 0) == -1) {
		puts("send failed");
		success = false;
	}

	return success;
}

void HttpServer::closeSocket()
{
	if (_socket != INVALID_SOCKET) {
		//printf("closing socket %d\n", _socket);
		closesocket(_socket);
	}
	_requestType = NotDefined;
	_socket = INVALID_SOCKET;
}

void HttpServer::parseCharValue(char* buff, const char* tag, int* value)
{
	const int CHAR_BUFF_LEN = 3;

	char* c = strstr(buff, tag);
	if (c != NULL) {
		c += strlen(tag);

		char str[CHAR_BUFF_LEN] = { '\0' };
		char* wp = str;
		int len = CHAR_BUFF_LEN;

		while ((*c != '\0') && --len) {
			*wp++ = *c++;
		}

		if (value != NULL) {
			*value = atoi(str);
		}
	}
}
