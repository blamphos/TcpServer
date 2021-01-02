#include "EventQueue.h"
#include "Spdif_defs.h"
#include "Parameters.h"
#include "HttpServer.h"

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
		case EVENT_HTTP_REQUEST_POST:
		{
			uint32_t level = (msg.data >> 3) & 0xFF;
			Spdif::InputTypeT input = static_cast<Spdif::InputTypeT>(msg.data & 0x3);
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
			}
			break;
		}
		case EVENT_HTTP_SEND_RESPONSE:
		{
			int index = static_cast<int>(Parameters::instance()->current_input);
			HttpResponseTypeT responseType = static_cast<HttpResponseTypeT>(msg.data);

			_http->sendResponse(responseType,
				Parameters::instance()->current_level,
				Parameters::instance()->current_input,
				Parameters::instance()->auto_find,
				Parameters::instance()->getSpdifStatus(index));
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
	static int volume = 26;
	static int input = 0;
	static bool auto_find = true;

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
