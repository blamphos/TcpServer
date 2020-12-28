#ifndef SPDIF_DEFS_H
#define SPDIF_DEFS_H

//#include "mbed.h"
#include <stdint.h>
#include <map>

namespace Spdif {
const int INPUT_COUNT = 3;

enum InputTypeT {
	Coax1 = 0,
	Coax2,
	Opt1,
	Auto
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

class SpdifStatus {
public:
	typedef struct {
		Spdif::InputTypeT input;
		Spdif::SampleRateT sample_rate;
		Spdif::PcmInfoTypeT pcm_info;
		uint8_t sdto_data;
	} spdif_message_t;

	static uint32_t create(spdif_message_t& msg);
	static spdif_message_t dispatch(uint32_t data);
	static std::map<Spdif::InputTypeT, const char*> inputTitleMap;
	static std::map<Spdif::SampleRateT, const char*> sampleRateTitleMap;
	static std::map<Spdif::PcmInfoTypeT, const char*> pcmInfoeTitleMap;

private:
	static const uint32_t SAMPLE_RATE_OFFSET;
	static const uint32_t PCM_INFO_OFFSET;
	static const uint32_t SDTO_DATA_OFFSET;
};

#endif
