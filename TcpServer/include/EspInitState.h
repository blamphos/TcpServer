#ifndef ESPINITSTATE_H
#define ESPINITSTATE_H

#include "TCPState.h"

class EspInitState : public TCPState {
public:
    EspInitState();
    virtual void onStateEnter(ESP8266* esp);
    virtual void handleMessage(ESP8266* esp, message_t msg);
    virtual void onStateExit(ESP8266* esp);
    virtual void processLine(ESP8266* esp);
    static TCPState* instance();

private:
    void initialize();
	void sendNextCommand(ESP8266* esp);

    const char* _ssid = "Kuhaverkko";
    const char* _pwd = "kasperi12";

	DigitalOut* _esp_reset;
	int _cmd_index;
};

#endif
