#ifndef SYSTEMCONTROL_H
#define SYSTEMCONTROL_H

#include "IO_mapping.h"
#include "Spdif_defs.h"
#include "Parameters.h"
#include "EmbeddedMessages.h"
#include "HttpServer.h"

class DisplayStateBase;
class VolumeControl;
class SpdifSwitch;
class IrReceiver;
class PushButtonSwitch;
class SpdifDecoder;
class RotaryEncoderSwitch;
class Parameters;
class Display;
class HttpServer;

class SystemControl {
public:
    ~SystemControl();
	void handleMessage();
	static SystemControl* instance();

private:
	enum TimerTypeT {
		TIMER_SYSTEM_TICK_US	= 100,
		TIMER_MASK_1_6_MS		= 0xF,
		TIMER_MASK_819_2_MS		= 0x1FFF
	};

	SystemControl();
	void ticker_isr();

	IrReceiver* _ir_receiver;
	SpdifDecoder* _spdif_decoder;
	VolumeControl* _volume_control;
	SpdifSwitch* _spdif_switch;
	PushButtonSwitch* _main_sw;
	PushButtonSwitch* _input_sw;
	RotaryEncoderSwitch* _rotary_sw;
	Ticker _system_ticker;

	Parameters* _parameters;
	Display* _display;
	HttpServer* _http;
};

#endif
