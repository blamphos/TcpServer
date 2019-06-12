#include <conio.h>
#include "mbed.h"
#include "ESP8266.h"
#include "HttpServer.h"

BusOut leds;
SerialDebug pc(p5, p6, 115200);

int main()
{
    ESP8266 esp;
    HttpServer http;

	pc.printf("GeVol 2.0 TFT\r\n");
	esp.initialize();

	while (1) {
		esp.handleMessage();
        if (_kbhit() != 0) {
            break;
        }
        wait_ms(10);
	}

	return 0;
}
