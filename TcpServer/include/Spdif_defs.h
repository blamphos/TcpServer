#ifndef SPDIF_DEFS_H
#define SPDIF_DEFS_H

#include "mbed.h"

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

	static uint32_t create(spdif_message_t& msg)
	{
		// EVENT_SPDIF_STATUS: message format
		// 19...08     | 07...04     | 03...00
		// PCM info    | SampleRateT | InputTypeT
		// PCM(3)      | SR(3)       | index(2)
		// 0000        | 0000        | 0000

		uint32_t msg_data = msg.input;
		msg_data |= (msg.sample_rate << SAMPLE_RATE_OFFSET);
		msg_data |= (msg.pcm_info << PCM_INFO_OFFSET);
		msg_data |= (msg.sdto_data << SDTO_DATA_OFFSET);

		return msg_data;
	}

	static spdif_message_t dispatch(uint32_t data)
	{
		spdif_message_t msg;
		msg.input = static_cast<Spdif::InputTypeT>(data & 0x3);
		msg.sample_rate = static_cast<Spdif::SampleRateT>((data >> SAMPLE_RATE_OFFSET) & 0x7);
		msg.pcm_info = static_cast<Spdif::PcmInfoTypeT>((data >> PCM_INFO_OFFSET) & 0x7);
		msg.sdto_data = (data >> SDTO_DATA_OFFSET);

		return msg;
	}

private:
	enum StatusMessageOffsetT {
		SAMPLE_RATE_OFFSET 	= 4,
		PCM_INFO_OFFSET 	= 8,
		SDTO_DATA_OFFSET 	= 12
	};
};
#endif
