#include "Parameters.h"

Parameters::Parameters() :
		auto_find(true),
		current_level(0),
		current_input(Spdif::Coax1)
{
	for (int i = 0; i < Spdif::INPUT_COUNT; ++i) {
		_spdif_status[i] = 0;
	}
}

bool Parameters::isMuted()
{
	return current_level == 0;
}

uint32_t Parameters::getSpdifStatus(Spdif::InputTypeT input)
{
	int index = static_cast<int>(input);

	return _spdif_status[index];
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
