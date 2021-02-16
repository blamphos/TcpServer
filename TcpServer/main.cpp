#include <iostream>
#include <list>
#include "EventQueue.h"
#include "HttpServer.h"
#include "Parameters.h"
#include "VolumeControl.h"

HttpServer* _http;

std::string cmdUStr = "84B4B0B434303000FF8484B4B0B084343484B030FF00FF00FF00FF00FF8484B4B0B084343484B030FF00FF00FF00FF8484B4B0B084343484B030FF00FF00FF00FF00FF8484B4B0B084343484B030FF00FF00FF00FF00FF8484B4B0B084343484B030FF00FF00FF00FF00FF8484B4B0B084343484B030FF00FF00FF00FF00FF84";
std::string cmdDStr = "84B4B0B084343484B030FF00FF00FF00FF00FF8484B4B0B0B034B430B030FF00FF00FF8484B4B0B0B034B430B030FF00FF00FF00FF8484B4B0B0B034B430B030FF00FF00FF00FF8484B4B0B0B034B430B030FF00FF00FF00FF8484B4B0B0B034B430B030FF00FF00FF00FF8484B4B0B0B034B430B030FF00FF00FF00FF8484B4";
std::string cmdMStr = "B0B0B034B430B030FF00FF00FF00A0C00F00803F190F74C00F58FE641F15C001A91F81F30300A8F8000080B40700000000580100FF8484B4B0B084B03484B030FF00FF00FF8484B4B0B084B03484B030FF00FF00FF8484B4B0B084B03484B030FF00FF00FF8484B4B0B084B03484B030FF00FF00FF8484B4B0B084B03484B030";

int kbhit(void)
{
	if (GetKeyState('X') & 0x8000/*Check if high-order bit is set (1 << 15)*/)
	{
		return 1;
	}

	return 0;
}

void threadFunc()
{
	message_t msg;
	bool running = true;
	
	while (running) {
		EventQueue::instance()->wait(msg);
		switch (msg.event) {
		case EVENT_HTTP_REQUEST_POST_SET_VOLUME:
		{
			int level = static_cast<int>(msg.data);
			if (level >= 0) {
				//uint32_t data = VolumeControl::Update | (level << 2);
				//EventQueue::instance()->post(EVENT_VOLUME_COMMAND, data);
				Parameters::instance()->current_level = level & 0xFF;
				EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_POST);
			}
			else {
				EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_INFO_UPDATE);
			}
			break;
		}
		case EVENT_HTTP_REQUEST_POST_SET_INPUT:
		{
			Spdif::InputTypeT input = Parameters::instance()->current_input;

			if (SpdifStatus::isValidInput(static_cast<int>(msg.data), &input))
			{
				Parameters::instance()->auto_find = false;
				if (input != Parameters::instance()->current_input) {
					//EventQueue::instance()->post(EVENT_CHANGE_INPUT_REQ, input);
					Parameters::instance()->current_input = input;
					EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_INFO_UPDATE);
				}
				else {
					EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_INFO_UPDATE);
				}
			}
			else {
				Parameters::instance()->auto_find = true;
				EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_INFO_UPDATE);
			}
			break;
		}
		case EVENT_HTTP_SEND_RESPONSE:
		{
			char buffer[64] = { '\0' };
			HttpResponseTypeT responseType = static_cast<HttpResponseTypeT>(msg.data);

			if (responseType == HTTP_RESPONSE_POST) {
				sprintf_s(buffer, "%d", Parameters::instance()->current_level);
			}
			else if (responseType == HTTP_RESPONSE_INFO_UPDATE) {
				char inputTitle[10] = { 0 };
				char sampleRateTitle[10] = { 0 };
				char pcmTitle[10] = { 0 };
				uint32_t data = Parameters::instance()->getSpdifStatus(Parameters::instance()->current_input);

				SpdifStatus::getInputInfo(data, inputTitle, sampleRateTitle, pcmTitle);
				sprintf_s(buffer, "%d;%s;%d;%s %s",
					Parameters::instance()->current_level,
					inputTitle,
					Parameters::instance()->auto_find ? 1 : 0,
					sampleRateTitle,
					pcmTitle);
			}
			_http->sendResponse(responseType, buffer);
			break;
		}
		case EVENT_SYSTEM_SHUTDOWN:
			puts("system shutdown");
			running = false;
			break;
		default:
			break;
		}
	}
}


std::vector<int> analyze(std::string command)
{
	bool store = false;
	std::vector<int> chunk;
	std::vector<std::vector<int>> chunks;

	for (size_t i = 0; i < command.length(); i += 2) {
		std::string s = command.substr(i, 2);
		int c = std::stoi(s, 0, 16);

		if (c == 0xFF) {
			store = true;
			if (chunk.size() >= 4) {
				chunks.push_back(chunk);
			}
			chunk.clear();
		}
		else if (store) {
			chunk.push_back(c);
		}
	}

	std::map<std::vector<int>, int> chunkBinMap;
	std::vector<std::vector<int>>::iterator iter = chunks.begin();
	std::vector<std::vector<int>>::iterator iterEnd = chunks.end();
	while (iter != iterEnd) {
		chunkBinMap[(*iter)]++;
		++iter;
	}

	using pair_type = decltype(chunkBinMap)::value_type;
	auto pr = std::max_element
	(
		std::begin(chunkBinMap), std::end(chunkBinMap),
		[](const pair_type& p1, const pair_type& p2) {
			return p1.second < p2.second;
		}
	);

	return pr->first;
}

void printCommand(std::vector<int> p_list)
{
	std::vector<int>::const_iterator cIter = p_list.begin();
	std::vector<int>::const_iterator cIterEnd = p_list.end();
	while (cIter != cIterEnd) {
		printf("%02X", (*cIter));
		++cIter;
	}
	printf("\n");
}

void getCodes(std::vector<int>& p_bytes, std::vector<uint32_t>& p_codes)
{
	if (p_bytes.size() >= 4) {
		int bytes = 0;
		uint32_t code = 0x0;

		std::vector<int>::const_iterator iter = p_bytes.begin();
		std::vector<int>::const_iterator iterEnd = p_bytes.end();
		while (iter != iterEnd) {
			code = (code << 8) | (*iter);
			++iter;
			if (++bytes == 4) {
				p_codes.push_back(code);
				break;
			}
		}

		p_bytes.erase(p_bytes.begin());
		getCodes(p_bytes, p_codes);
	}
}

std::vector<uint32_t> getUniqueCodes(std::vector<uint32_t>& p_uniqueCodes, std::vector<uint32_t>& p_codes)
{
	std::vector<uint32_t> codes;

	std::vector<uint32_t>::const_iterator iter = p_codes.begin();
	std::vector<uint32_t>::const_iterator iterEnd = p_codes.end();
	while (iter != iterEnd) {
		if (std::find(p_uniqueCodes.begin(), p_uniqueCodes.end(), (*iter)) != p_uniqueCodes.end()) {
			codes.push_back(*iter);
		}
		++iter;
	}

	return codes;
}

void trimCommand(std::vector<std::vector<int>>& p_matrix)
{
	std::map<int, int> sizeMap;

	std::vector<std::vector<int>>::const_iterator iter = p_matrix.begin();
	std::vector<std::vector<int>>::const_iterator iterEnd = p_matrix.end();
	while (iter != iterEnd) {
		sizeMap[(*iter).size()];
		++iter;
	}

	if (sizeMap.size() != 1) {
		return;
	}


}

void logBytes(std::vector<int> p_bytes)
{
	std::vector<int>::const_iterator iter = p_bytes.begin();
	std::vector<int>::const_iterator iterEnd = p_bytes.end();
	while (iter != iterEnd) {
		printf("%02X", (*iter));
		++iter;
	}
	printf("\n");
}

void logMatrix(std::vector<std::vector<int>> p_matrix)
{
	std::vector<std::vector<int>>::iterator iter = p_matrix.begin();
	std::vector<std::vector<int>>::iterator iterEnd = p_matrix.end();
	while (iter != iterEnd) {
		logBytes(*iter);
		++iter;
	}
}

void trimStart(std::vector<std::vector<int>>& p_matrix)
{
	std::map<int, int> dataSizes;
	std::vector<std::vector<int>>::iterator iter = p_matrix.begin();
	std::vector<std::vector<int>>::iterator iterEnd = p_matrix.end();
	while (iter != iterEnd) {
		dataSizes[(*iter).size()];
		++iter;
	}

	if (dataSizes.size() != 1) {
		return;
	}

	std::vector<int> copyIndexes;
	for (size_t i = 0; i < p_matrix[0].size(); i++) {
		std::map<int, int> byteMap;
		iter = p_matrix.begin();
		iterEnd = p_matrix.end();
		while (iter != iterEnd) {
			byteMap[(*iter)[i]];
			++iter;
		}

		if (byteMap.size() > 1) {
			iter = p_matrix.begin();
			iterEnd = p_matrix.end();
			while (iter != iterEnd) {
				(*iter).erase((*iter).begin(), (*iter).begin() + i);
				++iter;
			}
			break;
		}
	}
}

void reverseMatrix(std::vector<std::vector<int>>& p_matrix)
{
	std::vector<std::vector<int>>::iterator iter = p_matrix.begin();
	std::vector<std::vector<int>>::iterator iterEnd = p_matrix.end();
	while (iter != iterEnd) {
		std::reverse((*iter).begin(), (*iter).end());
		++iter;
	}
}

void trimEnd(std::vector<std::vector<int>>& p_matrix)
{
	reverseMatrix(p_matrix);
	trimStart(p_matrix);
	reverseMatrix(p_matrix);
}

void trim(std::vector<std::vector<int>>& p_matrix)
{
	trimStart(p_matrix);
	trimEnd(p_matrix);
}

enum IrCodesT {
	IR_VOL_UP,
	IR_VOL_DOWN,
	IR_VOL_MUTE
};

int __cdecl main(void)
{
	std::vector<std::vector<int>> commands;

	std::vector<int> upCmd = analyze(cmdUStr);
	commands.push_back(upCmd);
	//printCommand(upCmd);
	//std::vector<uint32_t> upCodes;
	//getCodeList(upCmd, upCodes);

	std::vector<int> downCmd = analyze(cmdDStr);
	commands.push_back(downCmd);
	//printCommand(downCmd);
	//std::vector<uint32_t> downCodes;
	//getCodeList(downCmd, downCodes);

	std::vector<int> muteCmd = analyze(cmdMStr);
	commands.push_back(muteCmd);
	//printCommand(muteCmd);
	//std::vector<uint32_t> muteCodes;
	//getCodeList(muteCmd, muteCodes);

	logMatrix(commands);
	trim(commands);
	logMatrix(commands);

	std::vector<std::vector<uint32_t>> irCodes;
	std::vector<std::vector<int>>::iterator iter = commands.begin();
	std::vector<std::vector<int>>::iterator iterEnd = commands.end();
	while (iter != iterEnd) {
		std::vector<uint32_t> codes;
		getCodes(*iter, codes);
		irCodes.push_back(codes);
		++iter;
	}

	std::map<IrCodesT, std::vector<uint32_t>> irCodesMap;
	irCodesMap[IR_VOL_UP] = irCodes[0];
	irCodesMap[IR_VOL_DOWN] = irCodes[1];
	irCodesMap[IR_VOL_MUTE] = irCodes[2];

	/*trimCommand(commands);

	std::map<uint32_t, int> codesMap;
	std::vector<uint32_t>::const_iterator iter = upCodes.begin();
	std::vector<uint32_t>::const_iterator iterEnd = upCodes.end();
	while (iter != iterEnd) {
		codesMap[(*iter)]++;
		++iter;
	}

	iter = downCodes.begin();
	iterEnd = downCodes.end();
	while (iter != iterEnd) {
		codesMap[(*iter)]++;
		++iter;
	}

	iter = muteCodes.begin();
	iterEnd = muteCodes.end();
	while (iter != iterEnd) {
		codesMap[(*iter)]++;
		++iter;
	}

	std::vector<uint32_t> uniqueCodes;
	std::map<uint32_t, int>::const_iterator mapIter = codesMap.begin();
	std::map<uint32_t, int>::const_iterator mapIterEnd = codesMap.end();
	while (mapIter != mapIterEnd) {
		if ((*mapIter).second == 1) {
			uniqueCodes.push_back((*mapIter).first);
		}
		++mapIter;
	}

	std::vector<uint32_t> uniqueUpCodes = getUniqueCodes(uniqueCodes, upCodes);
	std::vector<uint32_t> uniqueDownCodes = getUniqueCodes(uniqueCodes, downCodes);
	std::vector<uint32_t> uniqueMuteCodes = getUniqueCodes(uniqueCodes, muteCodes);
	*/

	//return 0;

	puts("GEVOL 3.0");
	puts("Print any key to exit...");
	bool b = Parameters::instance()->auto_find;
	VolumeControl volume(Parameters::instance()->getStartLevel());
	getchar();
	return 0;

	_http = new HttpServer();

	SpdifStatus::spdif_message_t msg;
	msg.input = Spdif::Coax1;
	msg.pcm_info = Spdif::PCM_NORMAL;
	msg.sample_rate = Spdif::SR_44100;	
	Parameters::instance()->setSpdifStatus(SpdifStatus::create(msg));

	msg.input = Spdif::Coax2;
	msg.pcm_info = Spdif::PCM_NORMAL;
	msg.sample_rate = Spdif::SR_192000;
	Parameters::instance()->setSpdifStatus(SpdifStatus::create(msg));

	msg.input = Spdif::Opt1;
	msg.pcm_info = Spdif::PCM_DD_AC3;
	msg.sample_rate = Spdif::SR_48000;
	Parameters::instance()->setSpdifStatus(SpdifStatus::create(msg));

	Parameters::instance()->current_level = 26;
	Parameters::instance()->current_input = Spdif::Coax1;
	Parameters::instance()->auto_find = true;

	std::thread t(threadFunc);

	printf("TCP server started!\n\n");
	while (!kbhit()) {
		Sleep(10);	
	}
	EventQueue::instance()->post(EVENT_SYSTEM_SHUTDOWN);
	t.join();

	delete _http;
	return 0;
}
