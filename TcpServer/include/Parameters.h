#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "Spdif_defs.h"

class Parameters {
public:
	bool isMuted();
	uint32_t getSpdifStatus(int index);
	void setSpdifStatus(uint32_t data);
	static Parameters* instance();

	bool auto_find;
	int current_level;
	Spdif::InputTypeT current_input;

private:
	Parameters();

	uint32_t _spdif_status[Spdif::INPUT_COUNT];
};

#endif
