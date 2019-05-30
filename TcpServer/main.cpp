#include <iostream>
#include "HttpServer.h"

int main()
{
	FILE* fp = fopen("/local/index.html", "r");
	if (fp == NULL) {
        printf("File 'index.html' not found!\n");
        return -1;
	}

    HttpServer::instance()->start();

    std::cin.get();
    HttpServer::instance()->stop();

	return 0;
}
