#ifndef VOLUME_CONTROL_H
#define VOLUME_CONTROL_H

#include "mbed.h"

class SoftSPI;

class VolumeControl {
public:
	enum VolumeCommandTypeT {
		Update = 0,
		Mute,
		VolumeUp,
		VolumeDown
	};

	enum VolumeLimitsT {
		VALUE_MIN = 0,
		VALUE_MAX = 255,
	};

	VolumeControl(PinName cs);
	void handleCommand(uint32_t command);
	void mute();
	void unmute();

private:
	enum ConstantsT {
		INIT_GAIN_VALUE = 0x34,

		LARGE_CHANGE = 4,
		SMALL_CHANGE = 2,

		MUTE_GUARD_PERIOD_MS = 500,
		CONTINUOUS_CMD_MS = 140,

		LOWER_LINEAR_RANGE_PER = 75,
		UPPER_LINEAR_RANGE_PER = 25
	};

	int spiWrite(int value);
	void setGain(int value);
	void toggleMute();

	//SoftSPI* _spi;
	DigitalOut _cs;
	int _gain_value;
	uint32_t _previous_gain;
	uint32_t _scaled_gain;
	bool _muted;
	Timer _guard_timer;
};

#endif
