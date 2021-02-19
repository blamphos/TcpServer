#include <sstream>
#include <algorithm>
#include "SpdifUtils.h"

std::map<Spdif::InputT, const char*> SpdifHelper::inputTitleMap = { 
	{Spdif::Coax1, "COAX1"},
	{Spdif::Coax2, "COAX2"},
	{Spdif::Opt1, "OPT1"},
};

std::map<Spdif::SampleRateT, const char*> SpdifHelper::sampleRateTitleMap = {
	{Spdif::SR_UNLOCK, "UNLOCK"},
	{Spdif::SR_44100, "44.1kHz"},
	{Spdif::SR_48000, "48kHz"},
	{Spdif::SR_96000, "96kHz"},
	{Spdif::SR_192000, "192kHz"},
};

std::map<Spdif::PcmInfoT, const char*> SpdifHelper::pcmInfoeTitleMap = {
	{Spdif::PCM_NO_INFO, ""},
	{Spdif::PCM_NORMAL, "PCM"},
	{Spdif::PCM_DD_AC3, "DD AC3"},
	{Spdif::PCM_DTS, "DTS"},
	{Spdif::PCM_NPCM, "NPCM"},
};

bool SpdifHelper::isValidInput(int index, Spdif::InputT* input)
{
	switch (index) {
	case 0:
	case 1:
	case 2:
		(*input) = static_cast<Spdif::InputT>(index);
		break;
	default:
		return false;
	}

	return true;
}

uint32_t SpdifHelper::serializeStatusMsg(Spdif::StatusS& msg)
{
	// EVENT_SPDIF_STATUS: message format
	// 19...08     | 07...04     | 03...00
	// PCM info    | SampleRateT | InputT
	// PCM(3)      | SR(3)       | index(2)
	// 0000        | 0000        | 0000

	uint32_t msg_data = msg.input;
	msg_data |= (msg.sample_rate << SAMPLE_RATE_OFFSET);
	msg_data |= (msg.pcm_info << PCM_INFO_OFFSET);
	msg_data |= (msg.sdto_data << SDTO_DATA_OFFSET);

	return msg_data;
}

Spdif::StatusS SpdifHelper::deserializeStatusMsg(uint32_t data)
{
	Spdif::StatusS msg;
	msg.input = static_cast<Spdif::InputT>(data & INPUT_BITS_MASK);
	msg.sample_rate = static_cast<Spdif::SampleRateT>((data >> SAMPLE_RATE_OFFSET) & 0x7);
	msg.pcm_info = static_cast<Spdif::PcmInfoT>((data >> PCM_INFO_OFFSET) & 0x7);
	msg.sdto_data = static_cast<uint8_t>(data >> SDTO_DATA_OFFSET);

	return msg;
}

void SpdifHelper::getInputInfo(uint32_t data, char* inputTitle, char* sampleRate, char* pcmInfo)
{
	Spdif::StatusS msg = deserializeStatusMsg(data);

	sprintf(inputTitle, "%s", inputTitleMap[msg.input]);
	sprintf(sampleRate, "%s", sampleRateTitleMap[msg.sample_rate]);
	sprintf(pcmInfo, "%s", pcmInfoeTitleMap[msg.pcm_info]);
}

std::vector<Spdif::InputT> SpdifHelper::parseSwitchPriorityOrder(std::string str)
{
	std::vector<Spdif::InputT> switchPriorityOrder;
	std::stringstream ss(str);

	for (int i; ss >> i;) {
		Spdif::InputT input;
		if (SpdifHelper::isValidInput(i, &input)) {
			switchPriorityOrder.push_back(input);
		}
		else {
			switchPriorityOrder.clear();
			break;
		}

		if (ss.peek() == ',') {
			ss.ignore();
		}
	}

	std::vector<Spdif::InputT>::iterator it = std::unique(switchPriorityOrder.begin(), switchPriorityOrder.end());
	if (it != switchPriorityOrder.end()) {
		switchPriorityOrder.clear();
	}

	return switchPriorityOrder;
}

