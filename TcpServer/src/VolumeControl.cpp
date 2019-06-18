#include "VolumeControl.h"
#include "Parameters.h"
//#include "SoftSPI.h"

VolumeControl::VolumeControl(PinName cs) :
		_cs(cs), _gain_value(INIT_GAIN_VALUE), _muted(false)
{
	//_spi = SoftSPI::instance();
	_cs = 1;

	setGain(_gain_value);
	_guard_timer.reset();
	_guard_timer.start();
}

void VolumeControl::handleCommand(uint32_t command)
{
	if (command == Update) {
	    _scaled_gain = Parameters::instance()->current_level;
        int gain = (_scaled_gain * 255) / 100;
        setGain(gain);
	} else {
		_guard_timer.stop();
		uint32_t cmd_period_ms = _guard_timer.read_ms();

		switch (command) {
		case Mute:
			//pc.printf("Mute\n");
			if (cmd_period_ms > MUTE_GUARD_PERIOD_MS) {
				toggleMute();
			}
			break;
		case VolumeUp:
			unmute();
			if (cmd_period_ms < CONTINUOUS_CMD_MS) {
				setGain(_gain_value + LARGE_CHANGE);
			} else {
				setGain(_gain_value + SMALL_CHANGE);
			}
			break;
		case VolumeDown:
			unmute();
			if (cmd_period_ms < CONTINUOUS_CMD_MS) {
				setGain(_gain_value - LARGE_CHANGE);
			} else {
				setGain(_gain_value - SMALL_CHANGE);
			}
			break;
		default:
			break;
		}

		_guard_timer.reset();
		_guard_timer.start();
	}
}

void VolumeControl::toggleMute()
{
	_muted = !_muted;

	if (_muted) {
		setGain(VALUE_MIN);
	} else {
		setGain(_previous_gain);
	}
}

void VolumeControl::mute()
{
	if (!_muted) {
		_muted = true;
		setGain(VALUE_MIN);
	}
}

void VolumeControl::unmute()
{
	if (_muted) {
		_muted = false;
		setGain(_previous_gain);
	}
}

void VolumeControl::setGain(int value)
{
	const int LINEAR_LOWER_GAIN_RANGE = LOWER_LINEAR_RANGE_PER * SMALL_CHANGE;
	const int NONLINEAR_UPPER_GAIN_RANGE = VALUE_MAX + 1 - LINEAR_LOWER_GAIN_RANGE;

	_gain_value = value;

	// Value range check
	if (value <= VALUE_MIN) {
		_gain_value = VALUE_MIN;
	} else if (value >= VALUE_MAX) {
		_gain_value = VALUE_MAX;
	}

	// Store new gain value only if not muted
	if (!_muted) {
		_previous_gain = _gain_value;
	}

	int response = spiWrite(_gain_value);
	if (response != _gain_value) {
		//pc.printf("SPI error: %d != %d\n", _gain_value, response);
	}

	// Calculate scaled gain for display according to given linear range
	if (_gain_value <= LINEAR_LOWER_GAIN_RANGE) {
		_scaled_gain = _gain_value * LOWER_LINEAR_RANGE_PER / LINEAR_LOWER_GAIN_RANGE;
	} else {
		_scaled_gain = (_gain_value - LINEAR_LOWER_GAIN_RANGE) * UPPER_LINEAR_RANGE_PER / NONLINEAR_UPPER_GAIN_RANGE;
		_scaled_gain += LOWER_LINEAR_RANGE_PER;
	}

	Parameters::instance()->current_level = _scaled_gain;
}

int VolumeControl::spiWrite(int value)
{
#ifdef SIMULATED
    return 0;
#else
	_cs = 0;
	int res = _spi->write(0x0);
	res = _spi->write(value);
	_cs = 1;

	return res;
#endif
}
