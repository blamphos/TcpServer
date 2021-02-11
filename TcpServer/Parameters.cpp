#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "Parameters.h"
#include "VolumeControl.h"

Parameters::Parameters() :
		auto_find(true),
		current_level(0),
		current_input(Spdif::Coax1)
{
	addInput(Spdif::Coax1);
	addInput(Spdif::Coax2);
	addInput(Spdif::Opt1);

	_configFile = "../home/pi/gevol.conf";

	if (!readConfigFile())
	{
		setDefaultDisplayType();
		setDefaultStartLevel();
		setDefaultInputName(Spdif::InputTypeT::Coax1);
		setDefaultInputName(Spdif::InputTypeT::Coax2);
		setDefaultInputName(Spdif::InputTypeT::Opt1);
		setDefaultAutoSwitch();
		setDefaultSwitchingOrder();
		writeConfigFile();
	}

	logParameters();
}

Parameters::~Parameters()
{
	std::map<Spdif::InputTypeT, SpdifInput*>::const_iterator iter = _inputs.begin();
	std::map<Spdif::InputTypeT, SpdifInput*>::const_iterator iterEnd = _inputs.end();
	while (iter != iterEnd) {
		delete (*iter).second;
		++iter;
	}
	_inputs.clear();
}

void Parameters::addInput(Spdif::InputTypeT input)
{
	SpdifInput* newInput = new SpdifInput(input);
	_inputs[input] = newInput;
}

bool Parameters::readConfigFile()
{
	std::ifstream cFile(_configFile);
	if (!cFile.is_open())
	{
		std::cerr << "Couldn't open config file for reading.\n";
		return false;
	}

	bool success = true;
	std::string line;
	while (getline(cFile, line)) {
		line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
		if (line[0] == '#' || line.empty()) {
			continue;
		}

		size_t delimiterPos = line.find("=");
		std::string key = line.substr(0, delimiterPos);
		std::string value = line.substr(delimiterPos + 1);

		if (key == "DisplayType") {
			if (value == "1") {
				_displayType = DisplayModuleT::ST7735S_RED_MODULE;
			}
			else if (value == "2") {
				_displayType = DisplayModuleT::ST7735S_BLUE_MODULE;
			}
			else {
				success = false;
				std::cerr << "Invalid display type, using default.\n";
			}
		}
		else if (key == "StartLevel") {
			int startLevel = -1;
			if (!(std::stringstream(value) >> startLevel)) {
				startLevel = -1;
			}

			if (VolumeControl::isValidLevel(startLevel)) {
				_startLevel = startLevel;
			}
			else {
				success = false;
				std::cerr << "Invalid start level, using default.\n";
			}
		}
		else if (key == "Input1") {
			if (!_inputs[Spdif::Coax1]->setName(value)) {
				success = false;
				std::cerr << "Invalid Input1 name, using default.\n";
			}
		}
		else if (key == "Input2") {
			if (!_inputs[Spdif::Coax2]->setName(value)) {
				success = false;
				std::cerr << "Invalid Input2 name, using default.\n";
			}
		}
		else if (key == "Input3") {
			if (!_inputs[Spdif::Opt1]->setName(value)) {
				success = false;
				std::cerr << "Invalid Input3 name, using default.\n";
			}
		}
		else if (key == "AutoSwitch") {
			if (value == "0") {
				auto_find = false;
			}
			else if(value == "1") {
				auto_find = true;
			}
			else {
				success = false;
				std::cerr << "Invalid autoswitch value, using default.\n";
			}
		}
		else if (key == "SwitchPriorityOrder") {
			_switchPriorityOrder.clear();
			std::stringstream ss(value);

			for (int i; ss >> i;) {
				Spdif::InputTypeT input;
				if (SpdifStatus::isValidInput(i - 1, &input)) {
					_switchPriorityOrder.push_back(input);
				}
				else {
					success = false;
					std::cerr << "Invalid SwitchPriorityOrder, using default.\n";
					break;
				}

				if (ss.peek() == ',') {
					ss.ignore();
				}
			}

			std::vector<Spdif::InputTypeT>::iterator it = std::unique(_switchPriorityOrder.begin(), _switchPriorityOrder.end());
			if (it != _switchPriorityOrder.end()) {
				success = false;
				setDefaultSwitchingOrder();
				std::cerr << "Duplicate inputs in SwitchPriorityOrder, using default.\n";
			}

			if (_switchPriorityOrder.size() != Spdif::INPUT_COUNT) {
				success = false;
				setDefaultSwitchingOrder();
				std::cerr << "Input count mismatch in SwitchPriorityOrder, using default.\n";
			}
		}
		else {
			success = false;
			std::cerr << "Unknown key: " << key << " (value: '" << value << "')\n";
		}
		//std::cout << key << " " << value << '\n';
	}

	return success;
}

bool Parameters::writeConfigFile()
{
	std::ofstream cFile(_configFile);
	if (!cFile.is_open())
	{
		std::cerr << "Couldn't open config file for writing.\n";
		return false;
	}

	if (_displayType == DisplayModuleT::ST7735S_RED_MODULE) {
		cFile << "DisplayType=1" << std::endl;
	}
	else if (_displayType = DisplayModuleT::ST7735S_BLUE_MODULE) {
		cFile << "DisplayType=2" << std::endl;
	}
	cFile << "StartLevel=" << _startLevel << std::endl;
	cFile << "Input1=" << _inputs[Spdif::InputTypeT::Coax1]->getName() << std::endl;
	cFile << "Input2=" << _inputs[Spdif::InputTypeT::Coax2]->getName() << std::endl;
	cFile << "Input3=" << _inputs[Spdif::InputTypeT::Opt1 ]->getName() << std::endl;
	cFile << "AutoSwitch=" << (auto_find ? "1" : "0") << std::endl;
	cFile << "SwitchPriorityOrder=" << getSwitchPriorityOrderListStr() << std::endl;
	cFile.close();

	return true;
}

void Parameters::logParameters()
{
	std::cout << "Display: " << _displayType << std::endl;
	std::cout << "StartLevel: " << _startLevel << std::endl;
	std::cout << "Input1: " << _inputs[Spdif::Coax1]->getName() << std::endl;
	std::cout << "Input2: " << _inputs[Spdif::Coax2]->getName() << std::endl;
	std::cout << "Input3: " << _inputs[Spdif::Opt1]->getName() << std::endl;
	std::cout << "AutoSwitch: " << auto_find << std::endl;
	std::cout << "SwitchPriorityOrder: ";
	std::vector<Spdif::InputTypeT>::const_iterator iter = _switchPriorityOrder.begin();
	std::vector<Spdif::InputTypeT>::const_iterator iterEnd = _switchPriorityOrder.end();
	while (iter != iterEnd) {
		std::cout << _inputs[(*iter)]->getName() << " ";
		++iter;
	}
	std::cout << std::endl;
	std::cout << "Current level: " << current_level << std::endl;
	std::cout << "Current input: " << _inputs[current_input]->getName() << std::endl;
}

void Parameters::setDefaultDisplayType()
{
	_displayType = DisplayModuleT::ST7735S_RED_MODULE;
}

void Parameters::setDefaultStartLevel()
{
	_startLevel = DEFAULT_START_LEVEL;
}

void Parameters::setDefaultInputName(Spdif::InputTypeT input)
{
	_inputs[input]->setDefaultName();
}

void Parameters::setDefaultAutoSwitch()
{
	auto_find = true;
}

void Parameters::setDefaultSwitchingOrder()
{
	_switchPriorityOrder.clear();
	_switchPriorityOrder.push_back(Spdif::InputTypeT::Coax1);
	_switchPriorityOrder.push_back(Spdif::InputTypeT::Opt1);
	_switchPriorityOrder.push_back(Spdif::InputTypeT::Coax2);
}

std::string Parameters::getSwitchPriorityOrderListStr()
{
	std::stringstream ss;

	std::vector<Spdif::InputTypeT>::const_iterator iter = _switchPriorityOrder.begin();
	std::vector<Spdif::InputTypeT>::const_iterator iterEnd = _switchPriorityOrder.end();
	while (iter != iterEnd) {
		ss << static_cast<int>(*iter + 1);
		if (iter < (iterEnd - 1)) {
			ss << ",";
		}
		++iter;
	}

	return ss.str();
}

bool Parameters::isMuted()
{
	return current_level == 0;
}

uint32_t Parameters::getSpdifStatus(Spdif::InputTypeT input)
{
	return _inputs[input]->getStatus();
}

void Parameters::setSpdifStatus(uint32_t data)
{
	Spdif::InputTypeT input = current_input;
	if (SpdifStatus::isValidInput(data & 0x3, &input)) {
		_inputs[input]->setStatus(data);
	}
}

int Parameters::getStartLevel()
{
	return _startLevel;
}

std::vector<Spdif::InputTypeT> Parameters::getSwitchingOrder()
{
	return _switchPriorityOrder;
}

bool Parameters::setSwitchingOrder(std::vector<Spdif::InputTypeT> list)
{
	std::vector<Spdif::InputTypeT>::iterator it = std::unique(list.begin(), list.end());
	if (it != list.end()) {
		std::cerr << "Duplicate inputs in SwitchPriorityOrder, no change.\n";
		return false;
	}

	if (list.size() != Spdif::INPUT_COUNT) {
		std::cerr << "Input count mismatch in SwitchPriorityOrder, no change.\n";
		return false;
	}

	_switchPriorityOrder.clear();
	_switchPriorityOrder = list;
	return true;
}

Parameters* Parameters::instance()
{
	static Parameters inst;
	return &inst;
}
