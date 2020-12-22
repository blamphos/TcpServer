#ifndef TCP_SOCKET_SERVER_H
#define TCP_SOCKET_SERVER_H

#include <winsock2.h>
#include "StoppableTask.h"

class TcpSocketServer : StoppableTask
{
protected:
	enum ConstantsT {
		RETRY_COUNT = 3
	};

	TcpSocketServer(uint16_t port);
	virtual ~TcpSocketServer();
	void run();
	void shutdown();

private:
	virtual void handleConnection(SOCKET socket) = 0;

	uint16_t _port;
	SOCKET _listenSocket;
	struct addrinfo* _server;
};

#endif
