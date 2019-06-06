#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "stdint.h"

namespace Spdif {
const int INPUT_COUNT = 3;

enum InputTypeT {
	Coax1 = 0,
	Coax2,
	Opt1,
	Auto
};
}

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
