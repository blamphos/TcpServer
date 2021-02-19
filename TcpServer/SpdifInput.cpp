#include "SpdifInput.h"

SpdifInput::SpdifInput(Spdif::InputT type) :
	_type(type), _startLevel(0), _status(0x0)
{
	setDefaultName();
}

bool SpdifInput::setName(std::string name)
{
	if (name.empty()) {
		return false;
	}

	_name = name;
	return true;
}

void SpdifInput::setDefaultName()
{
	switch (_type) {
	case Spdif::Coax1:
		_name = "COAX1";
		break;
	case Spdif::Coax2:
		_name = "COAX2";
		break;
	case Spdif::Opt1:
		_name = "OPT1";
		break;
	}
}

std::string SpdifInput::getName()
{
	return _name;
}

void  SpdifInput::setStatus(uint32_t status)
{
	_status = status;
}

uint32_t SpdifInput::getStatus()
{
	return _status;
}