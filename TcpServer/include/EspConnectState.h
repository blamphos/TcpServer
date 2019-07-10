#ifndef ESPCONNECTSTATE_H
#define ESPCONNECTSTATE_H

#include "EspStateBase.h"

class EspConnectState : public EspStateBase {
public:
    EspConnectState();
    virtual void onStateEnter(ESP8266* esp);
    virtual void handleMessage(ESP8266* esp, message_t msg);
    virtual void onStateExit(ESP8266* esp);
    virtual void processLine(ESP8266* esp);
    static EspStateBase* instance();

private:
    void timeout();

	uint32_t _expected_data_len;
};

#endif
