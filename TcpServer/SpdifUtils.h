#ifndef SPDIF_UTILS_H
#define SPDIF_UTILS_H

//#include "mbed.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <map>

namespace Spdif {
const int INPUT_COUNT = 3;

enum InputTypeT {
	Coax1 = 0,
	Coax2,
	Opt1,
};

enum SampleRateT {
	SR_UNLOCK = 0,
	SR_44100,
	SR_48000,
	SR_96000,
	SR_192000
};

enum PcmInfoTypeT {
	PCM_NO_INFO = 0,
	PCM_NORMAL,
	PCM_DD_AC3,
	PCM_DTS,
	PCM_NPCM
};
}

class SpdifHelper {
public:
	typedef struct {
		Spdif::InputTypeT input;
		Spdif::SampleRateT sample_rate;
		Spdif::PcmInfoTypeT pcm_info;
		uint8_t sdto_data;
	} spdif_message_t;

	static bool isValidInput(int index, Spdif::InputTypeT* input);
	static uint32_t create(spdif_message_t& msg);
	static spdif_message_t dispatch(uint32_t data);
	static void getInputInfo(uint32_t data, char* inputTitle, char* sampleRate, char* pcmInfo);
	static std::vector<Spdif::InputTypeT> parseSwitchPriorityOrder(std::string str);

private:
	enum SpdifMessageOffsetT {
		INPUT_BITS_MASK = 0x3,
		SAMPLE_RATE_OFFSET = 4,
		PCM_INFO_OFFSET = 8,
		SDTO_DATA_OFFSET = 12,
	};

	static std::map<Spdif::InputTypeT, const char*> inputTitleMap;
	static std::map<Spdif::SampleRateT, const char*> sampleRateTitleMap;
	static std::map<Spdif::PcmInfoTypeT, const char*> pcmInfoeTitleMap;
};

#endif
