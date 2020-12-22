#include "EventQueue.h"
#include "Spdif_defs.h"
#include "Parameters.h"
#include "HttpServer.h"

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
	HttpServer* _http = new HttpServer();

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
				}
			}
			else {
				//uint32_t data = VolumeControl::Update | (level << 2);
				//EventQueue::instance()->post(EVENT_VOLUME_COMMAND, data);
				EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_POST);
				Parameters::instance()->current_level = level;
			}
			break;
		}
		case EVENT_HTTP_SEND_RESPONSE:
		{
			HttpResponseTypeT responseType = static_cast<HttpResponseTypeT>(msg.data);
			_http->sendResponse(responseType,
				Parameters::instance()->current_level,
				Parameters::instance()->current_input);
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

	delete _http;
}

int __cdecl main(void)
{
	static int volume = 26;
	static int input = 0;
	static bool auto_find = true;

	puts("GEVOL 3.0");
	puts("Print any key to exit...");

	std::thread t(threadFunc);

	printf("TCP server started!\n\n");
	while (!kbhit()) {
		Sleep(10);	
	}
	EventQueue::instance()->post(EVENT_SYSTEM_SHUTDOWN);
	t.join();

	return 0;
}
