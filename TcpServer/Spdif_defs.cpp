#include "Spdif_defs.h"

const uint32_t SpdifStatus::SAMPLE_RATE_OFFSET = 4;
const uint32_t SpdifStatus::PCM_INFO_OFFSET = 8;
const uint32_t SpdifStatus::SDTO_DATA_OFFSET = 12;

uint32_t SpdifStatus::create(spdif_message_t& msg)
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

SpdifStatus::spdif_message_t SpdifStatus::dispatch(uint32_t data)
{
	spdif_message_t msg;
	msg.input = static_cast<Spdif::InputTypeT>(data & 0x3);
	msg.sample_rate = static_cast<Spdif::SampleRateT>((data >> SAMPLE_RATE_OFFSET) & 0x7);
	msg.pcm_info = static_cast<Spdif::PcmInfoTypeT>((data >> PCM_INFO_OFFSET) & 0x7);
	msg.sdto_data = static_cast<uint8_t>(data >> SDTO_DATA_OFFSET);

	return msg;
}
