#include "mbed.h"
#include "ESP8266.h"
#include "HttpServer.h"

BusOut leds;
Serial pc(p5, p6, 115200);

int main()
{
	pc.printf("GeVol 2.0 TFT\r\n");

    HttpServer http;
    ESP8266 esp(&http);
	esp.initialize();

	while (1) {
		esp.handleMessage();
        if (pc.waitKeyPress() != 0) {
            break;
        }
        wait_ms(10);
	}

	return 0;
}
