#include "HttpServer.h"
#include "HttpResponse.h"
#include "HttpResourceFile.h"
#include "EventQueue.h"
#include "SpdifUtils.h"

HttpServer::HttpServer() :
	TcpSocketServer(DEFAULT_PORT),
	_response(new HttpResponse(this)),
	_requestType(NotDefined),
	_socket(INVALID_SOCKET)
{
	_resourceIndexHtml = new HttpResourceFile("../home/pi/index.html", HttpResourceFile::TEXT_HTML);
	_resourceBackgroundJpg = new HttpResourceFile("../home/pi/background.jpg", HttpResourceFile::IMAGE_JPG);
	_resourceStyleCss = new HttpResourceFile("../home/pi/style.css", HttpResourceFile::TEXT_CSS);
	_resourceRsMinStyleCss = new HttpResourceFile("../home/pi/roundslider.min.css", HttpResourceFile::TEXT_CSS);
	_resourceRsMinScriptJs = new HttpResourceFile("../home/pi/roundslider.min.js", HttpResourceFile::TEXT_JAVASCRIPT);
	_resourceJQueryMinScriptJs = new HttpResourceFile("../home/pi/jquery-3.3.1.min.js", HttpResourceFile::TEXT_JAVASCRIPT);
	_resourceFaviconPng = new HttpResourceFile("../home/pi/favicon.png", HttpResourceFile::IMAGE_PNG);
	_resourceInputHtml = new HttpResourceFile("../home/pi/input.html", HttpResourceFile::TEXT_HTML);
}

HttpServer::~HttpServer()
{
	delete _response;
	delete _resourceIndexHtml;
	delete _resourceBackgroundJpg;
	delete _resourceStyleCss;
	delete _resourceRsMinStyleCss;
	delete _resourceRsMinScriptJs;
	delete _resourceJQueryMinScriptJs;
	delete _resourceFaviconPng;
	delete _resourceInputHtml;

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
			//break;
			closeSocket();
			return;
		}
		else {
			puts("error recv");
			closeSocket();
			return;
		}
	}
	
	if (_requestType == Post) {
		if (strstr(_buffer, "updateInfo") != NULL) {
			EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_INFO_UPDATE);
		}
		else if (strstr(_buffer, "updateSwitch") != NULL) {
			EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_SWITCH_UPDATE);
		}
		else if (strstr(_buffer, "volume=") != NULL) {
			int volume = -1;
			parseIntValue(_buffer, "volume=", &volume);

			uint32_t data = static_cast<int16_t>(volume) & 0xFFFF;
			EventQueue::instance()->post(EVENT_HTTP_REQUEST_POST_SET_VOLUME, data);
		}
		else if (strstr(_buffer, "input=") != NULL) {
			int input = -1;
			parseIntValue(_buffer, "input=", &input);

			uint32_t data = static_cast<int16_t>(input) & 0xFFFF;
			EventQueue::instance()->post(EVENT_HTTP_REQUEST_POST_SET_INPUT, data);
		}
		else if (strstr(_buffer, "switchOrder=") != NULL) {
			std::vector<Spdif::InputT> switchPriorityOrder;

			char* c = strstr(_buffer, "switchOrder=");
			if (c != NULL) {
				std::string str(c);
				size_t delimiterPos = str.find("=");
				std::string value = str.substr(delimiterPos + 1);
				switchPriorityOrder = SpdifHelper::parseSwitchPriorityOrder(value);
			}

			int shift = 2;
			uint32_t data = switchPriorityOrder.size() & 0x3;
			std::vector<Spdif::InputT>::const_iterator iter = switchPriorityOrder.begin();
			std::vector<Spdif::InputT>::const_iterator iterEnd = switchPriorityOrder.end();
			while (iter != iterEnd) {
				data |= ((*iter) & 03) << shift;
				shift += 2;
				++iter;
			}
			EventQueue::instance()->post(EVENT_HTTP_REQUEST_POST_SET_SWITCH_ORDER, data);
		}
		else  {
			puts("Unknown POST request");
			//puts(_buffer);
			EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_INFO_UPDATE);
		}

		int s = _sockets.dequeue();
	}
	else if (_requestType == Get) {
		if (strstr(_buffer, "GET / HTTP") != NULL) {
			EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_GET);
			int s =_sockets.dequeue();
		}
		
		if (strstr(_buffer, "GET /style.css") != NULL) {
			_response->send(_resourceStyleCss);
		}
		else if (strstr(_buffer, "GET /roundslider.min.css") != NULL) {
			_response->send(_resourceRsMinStyleCss);
		}
		else if (strstr(_buffer, "GET /roundslider.min.js") != NULL) {
			_response->send(_resourceRsMinScriptJs);
		}
		else if (strstr(_buffer, "GET /jquery-3.3.1.min.js") != NULL) {
			_response->send(_resourceJQueryMinScriptJs);
		}
		else if (strstr(_buffer, "GET /background.jpg") != NULL) {
			_response->send(_resourceBackgroundJpg);
		}
		else if (strstr(_buffer, "GET /favicon.png") != NULL) {
			_response->send(_resourceFaviconPng);
		}
		else if (strstr(_buffer, "GET /input.html") != NULL) {
			_response->send(_resourceInputHtml);
		}

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

void HttpServer::sendResponse(HttpResponseTypeT type, const char* buffer)
{
	if (_socket == INVALID_SOCKET) {
		puts("sendResponse: invalid socket");
		return;
	}

	int socket = _socket;
	_response->sendResponseOk();

	if (type == HttpResponseTypeT::HTTP_RESPONSE_GET) {
		_response->sendFile(_resourceIndexHtml);
	}
	else if (type == HttpResponseTypeT::HTTP_RESPONSE_POST) {
		sendBuffer(buffer, strlen(buffer));
	}
	else if (type == HttpResponseTypeT::HTTP_RESPONSE_INFO_UPDATE) {
		sendBuffer(buffer, strlen(buffer));
	}
	else if (type == HttpResponseTypeT::HTTP_RESPONSE_SWITCH_UPDATE) {
		sendBuffer(buffer, strlen(buffer));
	}

	closeSocket();

	_sockets.enqueue(socket);
}

bool HttpServer::sendBuffer(const char* buffer, int len)
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

void HttpServer::parseIntValue(char* buff, const char* tag, int* value)
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
