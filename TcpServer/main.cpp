#include <iostream>
#include "ESP8266Simulated.h"

int main()
{
	FILE* fp = fopen("/local/index.html", "r");
	if (fp == NULL) {
        printf("File 'index.html' not found!\n");
        return -1;
	}

    ESP8266Simulated::instance()->start();

    std::cin.get();
    ESP8266Simulated::instance()->stop();

	return 0;
}
