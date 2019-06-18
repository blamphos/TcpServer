#include "mbed.h"
#include "SystemControl.h"

Serial pc(USBTX, USBRX, 115200);
BusOut leds(LED1, LED2, LED3, LED4);

int main()
{
	pc.printf("GeVol 3.0 TFT\r\n");

	while (1) {
        SystemControl::instance()->handleMessage();

        if (pc.waitKeyPress() != 0) {
            break;
        }
	}

	return 0;
}
