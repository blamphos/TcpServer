#ifndef SPDIFINPUT_H
#define SPDIFINPUT_H

#include <string>
#include "Spdif_defs.h"

class SpdifInput {
public:
	SpdifInput(Spdif::InputTypeT type);
	bool setName(std::string name);
	void setDefaultName();
	std::string getName();
	void setStatus(uint32_t status);
	uint32_t getStatus();

private:
	Spdif::InputTypeT _type;
	int _startLevel;
	std::string _name;
	uint32_t _status;
};

#endif
