#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <vector> 
#include "SpdifUtils.h"
#include "SpdifInput.h"
#include "ST7735S_defs.h"

class Parameters {
public:
	bool isMuted();
	uint32_t getSpdifStatus(Spdif::InputT input);
	void setSpdifStatus(uint32_t data);
	int getStartLevel();
	std::vector<Spdif::InputT> getSwitchOrderList();
	bool setSwitchOrderList(std::vector<Spdif::InputT> list);
	std::string getSwitchOrderListStr();
	static Parameters* instance();

	bool auto_find;
	int current_level;
	Spdif::InputT current_input;

private:
	enum ConstantsT {
		DEFAULT_START_LEVEL = 26,
	};

	Parameters();
	~Parameters();
	void addInput(Spdif::InputT input);
	bool readConfigFile();
	bool writeConfigFile();
	void logParameters();

	void setDefaultDisplayType();
	void setDefaultStartLevel();
	void setDefaultInputName(Spdif::InputT input);
	void setDefaultAutoSwitch();
	void setDefaultSwitchingOrder();

	std::string _configFile;
	int _startLevel;
	DisplayModuleT _displayType;
	std::map<Spdif::InputT, SpdifInput*> _inputs;
	uint32_t _spdif_status[Spdif::INPUT_COUNT];
	std::vector<Spdif::InputT> _switchPriorityOrder;
};

#endif
