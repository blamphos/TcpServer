#ifndef SPDIF_UTILS_H
#define SPDIF_UTILS_H

//#include "mbed.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <map>

namespace Spdif {
	const int INPUT_COUNT = 3;

	enum InputT {
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

	enum PcmInfoT {
		PCM_NO_INFO = 0,
		PCM_NORMAL,
		PCM_DD_AC3,
		PCM_DTS,
		PCM_NPCM
	};

	typedef struct {
		Spdif::InputT input;
		Spdif::SampleRateT sample_rate;
		Spdif::PcmInfoT pcm_info;
		uint8_t sdto_data;
	} StatusS;
}

class SpdifHelper {
public:
	static bool isValidInput(int index, Spdif::InputT* input);
	static uint32_t serializeStatusMsg(Spdif::StatusS& msg);
	static Spdif::StatusS deserializeStatusMsg(uint32_t data);
	static void getInputInfo(uint32_t data, char* inputTitle, char* sampleRate, char* pcmInfo);
	static std::vector<Spdif::InputT> parseSwitchPriorityOrder(std::string str);

private:
	enum SpdifMessageOffsetT {
		INPUT_BITS_MASK = 0x3,
		SAMPLE_RATE_OFFSET = 4,
		PCM_INFO_OFFSET = 8,
		SDTO_DATA_OFFSET = 12,
	};

	static std::map<Spdif::InputT, const char*> inputTitleMap;
	static std::map<Spdif::SampleRateT, const char*> sampleRateTitleMap;
	static std::map<Spdif::PcmInfoT, const char*> pcmInfoeTitleMap;
};

#endif
