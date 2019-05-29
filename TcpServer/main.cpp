#include <stdio.h>
#include <conio.h>

#include "HttpServer.h"

int main(void)
{
    HttpServer::instance()->start();

	FILE* fp = fopen("/local/index.html", "r");
	if (fp == NULL) {
        printf("File 'index.html' not found!\n");
        return -1;
	}

    //std::thread t1(serverThread);

    getch();
    HttpServer::instance()->stop();
	//WSACleanup();
	return 0;
}
