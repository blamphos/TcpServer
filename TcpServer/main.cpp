#include "mbed.h"
#include "ESP8266.h"
#include "ESP8266Simulated.h"

BusOut leds;
//RawSerial pc(p5, p6, 115200);
ESP8266 esp;

int main()
{
	//pc.attach(&pc_rx_isr, Serial::RxIrq);
	printf("GeVol 2.0 TFT\r\n");

	esp.initialize();

	/*while (1) {
		esp.handleMessage();
	}*/

    std::cin.get();
    ESP8266Simulated::instance()->stop();

	return 0;
}
