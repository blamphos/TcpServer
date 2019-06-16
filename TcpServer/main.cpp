#include "mbed.h"
#include "HttpServer.h"

BusOut leds;
Serial pc(p5, p6, 115200);

int main()
{
	pc.printf("GeVol 3.0 TFT\r\n");

    HttpServer http;

	while (1) {
        message_t msg;
        if (EventQueue::instance()->getNext(msg)) {
            http.handleMessage(msg);
        }

        if (pc.waitKeyPress() != 0) {
            break;
        }
        wait_ms(10);
	}

	return 0;
}
