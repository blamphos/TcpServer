#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <vector> 
#include "Spdif_defs.h"
#include "SpdifInput.h"
#include "ST7735S_defs.h"

class Parameters {
public:
	bool isMuted();
	uint32_t getSpdifHelper(Spdif::InputTypeT input);
	void setSpdifHelper(uint32_t data);
	int getStartLevel();
	std::vector<Spdif::InputTypeT> getSwitchOrderList();
	bool setSwitchOrderList(std::vector<Spdif::InputTypeT> list);
	std::string getSwitchOrderListStr();
	static Parameters* instance();

	bool auto_find;
	int current_level;
	Spdif::InputTypeT current_input;

private:
	enum ConstantsT {
		DEFAULT_START_LEVEL = 26,
	};

	Parameters();
	~Parameters();
	void addInput(Spdif::InputTypeT input);
	bool readConfigFile();
	bool writeConfigFile();
	void logParameters();

	void setDefaultDisplayType();
	void setDefaultStartLevel();
	void setDefaultInputName(Spdif::InputTypeT input);
	void setDefaultAutoSwitch();
	void setDefaultSwitchingOrder();

	std::string _configFile;
	int _startLevel;
	DisplayModuleT _displayType;
	std::map<Spdif::InputTypeT, SpdifInput*> _inputs;
	uint32_t _spdif_status[Spdif::INPUT_COUNT];
	std::vector<Spdif::InputTypeT> _switchPriorityOrder;
};

#endif
