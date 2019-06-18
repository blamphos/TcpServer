#ifndef SPDIF_SWITCH_H
#define SPDIF_SWITCH_H

#include "mbed.h"
#include "Spdif_defs.h"

class Parameters;

class SpdifSwitch {
public:
	SpdifSwitch(PinName inp1, PinName inp2, PinName inp3);
	void mute();
	void unmute();
	void updateMuteState();
	void switchInput(uint32_t data);
	void updateSpdifStatus(uint32_t data);

private:
	void setActiveInput(Spdif::InputTypeT input);

	BusOut* _switch_ctrl;
	Parameters* _parameters;
};

#endif
