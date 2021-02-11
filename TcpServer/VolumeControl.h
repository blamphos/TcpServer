#ifndef VOLUME_CONTROL_H
#define VOLUME_CONTROL_H

//#include "mbed.h"
//#include "IO_mapping.h"
#include <stdint.h>
#include <map>

class VolumeControl {
public:
	enum VolumeCommandTypeT {
		Update = 0,
		Mute,
		VolumeUp,
		VolumeDown
	};

	VolumeControl();
	void handleCommand(uint32_t data);
	static bool isValidLevel(int level);
	void mute();
	void unmute();

private:
	enum ConstantsT {
		GAIN_VALUE_MIN = 0,
		GAIN_VALUE_MAX = 255,
		GAIN_RANGE_LINEAR_MAX = 150,

		LEVEL_VALUE_MIN = 0,
		LEVEL_VALUE_MAX = 99,
		LEVEL_RANGE_LINEAR_MAX = 75,

		SMALL_LEVEL_CHANGE = 1,
		LARGE_LEVEL_CHANGE = 3,

		MUTE_GUARD_PERIOD_MS = 500,
		CONTINUOUS_CMD_GATE_PERIOD_MS = 140
	};

	bool writeNewGain();
	void updateLevel(int level);
	void toggleMute();

	//DigitalOut _cs;
	int _level;
	int _previous_level;
	bool _muted;
	//Timer _guard_timer;

	std::map<int, int> _gainMap;
};

#endif
