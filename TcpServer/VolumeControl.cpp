#include <math.h>
#include "VolumeControl.h"
#include "Parameters.h"
//#include "SoftSPI.h"

VolumeControl::VolumeControl(int initLevel) :
		/*_cs(CE0),*/ _muted(false)
{
	//_cs = 1;

	// Initialize gain map
	_gainMap.clear();

	const double SMALL_STEP = GAIN_RANGE_LINEAR_MAX / LEVEL_RANGE_LINEAR_MAX;
	const double LARGE_STEP = (double)(GAIN_VALUE_MAX - GAIN_RANGE_LINEAR_MAX) / (double)(LEVEL_VALUE_MAX - LEVEL_RANGE_LINEAR_MAX);

	int gain = 0;
	for (int level = 0; level <= LEVEL_VALUE_MAX; level++) {
		// Calculate level shown for UI according to given gain ranges
		if (level <= LEVEL_RANGE_LINEAR_MAX) {
			gain = static_cast<int>(floor(level * SMALL_STEP));
		}
		else {
			gain = GAIN_RANGE_LINEAR_MAX + static_cast<int>(floor((level - LEVEL_RANGE_LINEAR_MAX) * LARGE_STEP));
		}
		_gainMap[level] = gain & 0xFF;
	}
	
	if (initLevel > 30) {
		initLevel = 30;
	}
	updateLevel(initLevel);

	//_guard_timer.reset();
	//_guard_timer.start();
}

void VolumeControl::handleCommand(uint32_t data)
{
	VolumeCommandTypeT command = static_cast<VolumeCommandTypeT>(data & 0x3);

	if (command == Update) {
		int level = static_cast<int>((data >> 2) & 0xFF);
		if (level == 0) {
			toggleMute();
		}
		else {
			updateLevel(level);
		}
	} 
	else {
		//_guard_timer.stop();
		uint32_t cmd_period_ms = 0;// _guard_timer.read_ms();

		switch (command) {
		case Mute:
			//pc.printf("Mute\n");
			if (cmd_period_ms > MUTE_GUARD_PERIOD_MS) {
				toggleMute();
			}
			break;
		case VolumeUp:
			unmute();
			if (cmd_period_ms < CONTINUOUS_CMD_GATE_PERIOD_MS) {
				updateLevel(_level + LARGE_LEVEL_CHANGE);
			} else {
				updateLevel(_level + SMALL_LEVEL_CHANGE);
			}
			break;
		case VolumeDown:
			unmute();
			if (cmd_period_ms < CONTINUOUS_CMD_GATE_PERIOD_MS) {
				updateLevel(_level - LARGE_LEVEL_CHANGE);
			} else {
				updateLevel(_level - SMALL_LEVEL_CHANGE);
			}
			break;
		default:
			break;
		}

		//_guard_timer.reset();
		//_guard_timer.start();
	}
}

bool VolumeControl::isValidLevel(int level)
{
	if (level < LEVEL_VALUE_MIN) {
		return false;
	}
	else if (level > LEVEL_VALUE_MAX) {
		return false;
	}

	return true;
}

void VolumeControl::toggleMute()
{
	_muted = !_muted;

	if (_muted) {
		updateLevel(LEVEL_VALUE_MIN);
	} else {
		updateLevel(_previous_level);
	}
}

void VolumeControl::mute()
{
	if (!_muted) {
		_muted = true;
		updateLevel(LEVEL_VALUE_MIN);
	}
}

void VolumeControl::unmute()
{
	if (_muted) {
		_muted = false;
		updateLevel(_previous_level);
	}
}

void VolumeControl::updateLevel(int level)
{
	_level = level;

	// Value range check
	if (_level <= LEVEL_VALUE_MIN) {
		_level = LEVEL_VALUE_MIN;
	} else if (_level >= LEVEL_VALUE_MAX) {
		_level = LEVEL_VALUE_MAX;
	}

	// Store new level value only if not muted
	if (!_muted) {
		_previous_level = _level;
	}

	// Write actual gain value according to level using SPI
	if (!writeNewGain()) {
		//printf("SPI error: %d != %d\n", _gain_value, response);
	}

	if (_muted && _level > 0) {
		_muted = false;
	}

	Parameters::instance()->current_level = _level;
	printf("VolumeControl::setGain: %d\n", Parameters::instance()->current_level);
}

bool VolumeControl::writeNewGain()
{
	int gain = _gainMap[_level];

	if (gain <= GAIN_VALUE_MIN) {
		gain = LEVEL_VALUE_MIN;
	}
	else if (gain >= GAIN_VALUE_MAX) {
		gain = GAIN_VALUE_MAX;
	}

	//_cs.write(0);
	//SoftSPI::instance()->write(0x00);
	int readGain = gain;// SoftSPI::instance()->write(gain & 0xFF);
	//_cs.write(1);

	return readGain == gain;
}
