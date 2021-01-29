#include "EventQueue.h"
#include "HttpServer.h"
#include "Parameters.h"

HttpServer* _http;

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
			/*Spdif::InputTypeT input = static_cast<Spdif::InputTypeT>(msg.data & 0x3);
			bool valid = msg.data & 0x4;
			printf("level: %d, input: %d (valid: %d)\n", level, input, valid);
			if (valid) {
				if (input == Parameters::instance()->current_input) {
					EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_POST);
				}
				else {
					//EventQueue::instance()->post(EVENT_CHANGE_INPUT_REQ, input);
					Parameters::instance()->current_input = input;
					EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_INFO_UPDATE);
				}
			}
			else {
				//uint32_t data = VolumeControl::Update | (level << 2);
				//EventQueue::instance()->post(EVENT_VOLUME_COMMAND, data);
				Parameters::instance()->current_level = level;
				EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_POST);
			}*/
			break;
		}
		case EVENT_HTTP_REQUEST_POST_SET_INPUT:
		{
			//HttpResponseTypeT responseType = HttpResponseTypeT::HTTP_RESPONSE_INFO_UPDATE;
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
				sprintf(buffer, "%d", Parameters::instance()->current_level);
			}
			else if (responseType == HTTP_RESPONSE_INFO_UPDATE) {
				char inputTitle[10] = { 0 };
				char sampleRateTitle[10] = { 0 };
				char pcmTitle[10] = { 0 };
				uint32_t data = Parameters::instance()->getSpdifStatus(Parameters::instance()->current_input);

				SpdifStatus::getInputInfo(data, inputTitle, sampleRateTitle, pcmTitle);
				sprintf(buffer, "%d;%s;%d;%s %s",
					Parameters::instance()->current_level,
					inputTitle,
					Parameters::instance()->auto_find ? 1 : 0,
					sampleRateTitle,
					pcmTitle);
			}
			_http->sendResponse(responseType, buffer);
			/*_http->sendResponse(responseType,
				Parameters::instance()->current_level,
				Parameters::instance()->current_input,
				Parameters::instance()->auto_find,
				Parameters::instance()->getSpdifStatus(index));*/
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

int __cdecl main(void)
{
	puts("GEVOL 3.0");
	puts("Print any key to exit...");

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
