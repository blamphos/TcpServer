#include "Parameters.h"

Parameters::Parameters() :
		auto_find(true),
		current_level(0),
		current_input(Spdif::Auto)
{
	for (int i = 0; i < Spdif::INPUT_COUNT; ++i) {
		_spdif_status[i] = 0;
	}
}

bool Parameters::isMuted()
{
	return current_level == 0;
}

uint32_t Parameters::getSpdifStatus(int index)
{
	if (index < Spdif::INPUT_COUNT) {
		return _spdif_status[index];
	}

	return 0;
}

void Parameters::setSpdifStatus(uint32_t data)
{
	int index = data & 0x3;
	if (index < Spdif::INPUT_COUNT) {
		_spdif_status[index] = data;
	}
}

Parameters* Parameters::instance()
{
	static Parameters inst;
	return &inst;
}
