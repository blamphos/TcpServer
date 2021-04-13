#include <iostream>
#include <list>
#include <sstream>
#include <conio.h>
#include <cmath>
#include "EventQueue.h"
#include "HttpServer.h"
#include "Parameters.h"
#include "VolumeControl.h"
#include "Callback.h"
#include "IrAnalyxerUtils.h"
#include <windows.h>
#include "GFX_Fonts.h"
#include "ST7735S.h"

HttpServer* _http;
ST7735S* _tft;

int kbHit(void)
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
			Spdif::InputT input = Parameters::instance()->current_input;

			if (SpdifHelper::isValidInput(static_cast<int>(msg.data), &input))
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
		case EVENT_HTTP_REQUEST_POST_SET_SWITCH_ORDER:
		{
			std::stringstream ss;

			int listSize = msg.data & 0x3;
			for (int i = 0; i < listSize; i++) {
				msg.data = msg.data >> 2;
				ss << (msg.data & 0x3);
				if (i < (listSize - 1)) {
					ss << ",";
				}
			}

			std::vector<Spdif::InputT> switchPriorityOrder = SpdifHelper::parseSwitchPriorityOrder(ss.str());
			Parameters::instance()->setSwitchOrderList(switchPriorityOrder);

			EventQueue::instance()->post(EVENT_HTTP_SEND_RESPONSE, HTTP_RESPONSE_SWITCH_UPDATE);
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

				SpdifHelper::getInputInfo(data, inputTitle, sampleRateTitle, pcmTitle);
				sprintf_s(buffer, "%d;%s;%d;%s %s",
					Parameters::instance()->current_level,
					inputTitle,
					Parameters::instance()->auto_find ? 1 : 0,
					sampleRateTitle,
					pcmTitle);
			}
			else if (responseType == HTTP_RESPONSE_SWITCH_UPDATE) {
				std::string list = Parameters::instance()->getSwitchOrderListStr();
				sprintf_s(buffer, "%s", list.c_str());
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

class ISegmentItem {
public:
	ISegmentItem() :
		_min(0), _x(0), _y(0)
	{

	}

	virtual void update(int value) = 0;

protected:
	int _min;
	int _x;
	int _y;
};

class RingSegmentItem {
public:
	RingSegmentItem(int p_index, int p_segs) : 
		_startAngle(0), _min(0), _size(4)
	{
		_startAngle = p_index * 15 - 90;
		_min = p_index * (100 / p_segs);

		int radius = 60;
		_x = 81 + radius * cos((_startAngle - 90) * DEG2RAD);
		_y = 72 + radius * sin((_startAngle - 90) * DEG2RAD);
		
		_tft->fillCircle(_x, _y, _size, ST7735S_DARKGREY);
	}

	void update(int p_level)
	{
		if (p_level >= _min) {
			_tft->fillCircle(_x, _y, _size, ST7735S_WHITE);
		}
		else {
			_tft->fillCircle(_x, _y, _size, ST7735S_DARKGREY);
		}
	}
	
private:
	int _startAngle;
	int _min;
	int _x;
	int _y;
	int _size;
};

class BarItem : public ISegmentItem {
public:
	BarItem(int p_index, int p_bars)
	{
		const int HEIGHT = 5;

		_min = p_index * (100 / p_bars);

		int max = (p_index + 1) * (100 / p_bars) - 1;
		_h = HEIGHT;

		_x = 8 + 14 * p_index;
		_y = 98;
	}

	void update(int p_level)
	{
		if (p_level && p_level >= _min) {
			_tft->fillRect(_x, _y, 12, _h, ST7735S_WHITE);
		}
		else {
			_tft->fillRect(_x, _y, 12, _h, ST7735S_BLACK);
			_tft->drawRect(_x, _y, 12, _h, ST7735S_DARKGREY);
		}
	}

private:
	int _h;
};

class BarItem2 : public ISegmentItem {
public:
	BarItem2(int p_index, int p_bars)
	{
		const int HEIGHT = 3;
		const int WIDTH = 6;

		_min = p_index * (100 / p_bars);

		int max = (p_index + 1) * (100 / p_bars) - 1;
		_h = HEIGHT;
		_w = WIDTH;

		_x = 14 + _w * p_index;
		_y = 90;
		_dimmed = true;
	}

	void update(int p_level)
	{
		if (p_level && p_level >= _min) {
			if (_dimmed) {
				_tft->fillRect(_x, _y, _w, _h, ST7735S_WHITE);
				_dimmed = false;
			}
		}
		else {
			if (!_dimmed) {
				_tft->fillRect(_x, _y, _w, _h, ST7735S_DARKGREY);
				_dimmed = true;
			}
		}
	}

private:
	int _h;
	int _w;
	bool _dimmed;
};

const char lanIconBitmap24x20[80] = {
	0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFE, 0x00, 0x7F, 0xFF, 0xFE, 0x00,
	0x60, 0x00, 0x06, 0x00, 0x60, 0x00, 0x06, 0x00, 0x67, 0xFF, 0xE6, 0x00,
	0x64, 0xDB, 0x26, 0x00, 0x64, 0x9B, 0x26, 0x00, 0x66, 0xDB, 0x66, 0x00,
	0x67, 0xFF, 0xE6, 0x00, 0x67, 0xFF, 0xE6, 0x00, 0x67, 0xFF, 0xE6, 0x00,
	0x61, 0xFF, 0x86, 0x00, 0x60, 0x7E, 0x06, 0x00, 0x60, 0x3C, 0x06, 0x00,
	0x60, 0x3C, 0x06, 0x00, 0x60, 0x00, 0x06, 0x00, 0x7F, 0xFF, 0xFE, 0x00,
	0x7F, 0xFF, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00
};

const char wifiIconBitmap24x24[96] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFE, 0x00, 0x00,
	0x07, 0xFF, 0xC0, 0x00, 0x0F, 0xC7, 0xE0, 0x00, 0x1E, 0x00, 0xF0, 0x00,
	0x38, 0x30, 0x78, 0x00, 0x31, 0xFE, 0x38, 0x00, 0x03, 0xFF, 0x00, 0x00,
	0x07, 0xC7, 0x80, 0x00, 0x07, 0x03, 0xC0, 0x00, 0x02, 0x01, 0x80, 0x00,
	0x00, 0x38, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00, 0x00, 0x7C, 0x00, 0x00,
	0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

class Label {
public:
	Label(int p_x, int p_y, uint16_t p_color, uint16_t p_backColor, GFXfont p_font) :
		_x(p_x), _y(p_y), _width(0), _height(0), _color(p_color), _backColor(p_backColor), _font(p_font), _prevChar(0)
	{
		// Scan glyphs to set the (max) height
		for (int i = 0; i < (_font.last - _font.first); i++) {
			switch (static_cast<char>(i + 32))
			{
			case '0': case '1': case '2': case '3': case '4': 
			case '5': case '6': case '7': case '8': case '9':
				if (_font.glyph[i].height > _height) {
					_height = _font.glyph[i].height;
				}
				if (_font.glyph[i].xAdvance > _width) {
					_width = _font.glyph[i].xAdvance;
				}
				break;
			default:
				break;
			}
		}
	}

	void setText(const char* p_str)
	{
		//SetTextColor(_color, _backColor);

		if (_width > 0) {
			_tft->fillRect(_x, _y - _height, _width, _height, _backColor);
		}

		_width = _tft->drawStringGFX(p_str, _x, _y, _font);
	}

	void setChar(int p_char)
	{
		if (p_char == _prevChar) {
			return;
		}
		_prevChar = p_char;

		if (_width > 0) {
			_tft->fillRect(_x, _y - _height, _width, _height, _backColor);
		}

		char c = static_cast<char>(p_char);
		int xOffset = (_width - _font.glyph[p_char - 32].xAdvance) / 2;
		_tft->drawCharGFX(c, _x + xOffset, _y, _font);
	}

private:
	int _x;
	int _y;
	int _width;
	int _height;
	uint16_t _color;
	uint16_t _backColor;
	GFXfont _font;
	int _prevChar;
};

class SlidingBar
{
public:
	SlidingBar(int p_x0, int p_y0, int p_width, int p_height) : 
		_x0(p_x0), _y0(p_y0), _width(p_width), _height(p_height), _lastLevel(0)
	{

	}

	void update(int p_level)
	{
		if (p_level == _lastLevel) {
			return;
		}

		if (_lastLevel == 0) {
			_tft->drawRect(_x0, _y0, _width, _height, ST7735S_DARKGREY);
		}

		int barWidth = _width * p_level / 99;
		int diff = p_level - _lastLevel;

		if (diff >= 0) {
			_tft->fillRect(_x0, _y0, barWidth, _height, ST7735S_WHITE);
		}
		else {
			_tft->fillRect(_x0 + barWidth, _y0 + 1, _width - barWidth, _height - 2, ST7735S_BLACK);
			_tft->drawRect(_x0 + barWidth, _y0, _width - barWidth, _height, ST7735S_DARKGREY);
		}

		_lastLevel = p_level;
	}

private:
	int _x0;
	int _y0;
	int _width;
	int _height;
	int _lastLevel;
};

void draw()
{
	_tft->fillRect(0, 0, 162, 132, ST7735S_BLACK);
	_tft->drawRect(0, 0, 162, 132, ST7735S_WHITE);

	if (false) {
		int r = 20;
		for (int i = 0; i < 5; i++) {
			_tft->fillArc(81, 61, 60, 10, r, r, 2, ST7735S_WHITE);
			r += 5;
		}
		return;
	}

	if (false) {
		int x = 8;
		for (int i = 0; i < 10; i++) {
			//fillArc(81, 61, 60, 10, r, r, 2, ST7735S_WHITE);
			_tft->fillRect(x, 80, 12, 10, ST7735S_WHITE);
			x += 14;
		}
		return;
	}

	if (false) {
		int vx = 6;
		int vy = 134;
		//drawStringGFX("COAX2", vx, vy, Roboto_Mono_32);
		_tft->drawStringGFX("192kHz PCM 0123456789", 170, 10, Roboto_Mono_16);
		_tft->drawStringGFX("192kHz PCM 0123456789", 170, 30, Roboto_Mono_32);
		_tft->drawStringGFX("0123456789", 170, 70, Roboto_Mono_64);

		//drawStringGFX("192KHZ PCM 0123456789", 400, 140, Open_Sans_Regular_16);
		_tft->drawStringGFX("192 kHz PCM 0123456789", 400, 140, Open_Sans_Light_16);
		_tft->drawStringGFX("COAX2 0123456789", 400, 160, Open_Sans_Regular_24);
		_tft->drawStringGFX("COAX2 0123456789", 400, 190, Open_Sans_Regular_32);
		_tft->drawStringGFX("COAX2 0123456789", 400, 220, Open_Sans_Regular_64);
		_tft->drawStringGFX("0123456789", 400, 290, Orbitron_Medium_64);
	}

	//drawBitmap(lanIconBitmap24x20, 110, 6, 24, 20);
	//drawBitmap(wifiIconBitmap24x24, 130, 3, 24, 20);

	_tft->drawRect(5, 28, 152, 1, ST7735S_WHITE);
	_tft->drawStringGFX("SWITCH INPUT", 8, 24, Open_Sans_Light_16);
	_tft->drawStringGFX("COAX1", 20, 48, Open_Sans_Light_16);
	_tft->drawStringGFX("COAX2", 20, 72, Open_Sans_Light_16);
	_tft->drawStringGFX("OPT1", 20, 96, Open_Sans_Light_16);
	_tft->drawStringGFX("Exit (AUTO)", 20, 120, Open_Sans_Light_16);
	getchar();

	if (true) {
		int xPos = 8;
		//_text_color = ST7735S_CYAN;
		//xPos += drawString("COAX2", xPos, 102, ST7735S_FONT32);
		//drawStringGFX("COAX2", xPos, 126, Roboto_Mono_32);
		//drawStringGFX("COAX2", xPos, 122, Open_Sans_Regular_24);
		Label inputLabel(8, 122, ST7735S_CYAN, ST7735S_BLACK, Open_Sans_Regular_28);
		inputLabel.setText("COAX2");


		if (Parameters::instance()->auto_find) {
			//drawStringGFX("AUTO", 116, 122, Open_Sans_Light_12);
			Label autoLabel(116, 122, ST7735S_CYAN, ST7735S_BLACK, Open_Sans_Light_12);
			autoLabel.setText("AUTO");
			//drawRect(116, 102, 40, 20, ST7735S_CYAN);
		}

		xPos = 8;
		xPos += _tft->drawStringGFX("44.1k ", xPos, 24, Open_Sans_Light_16);
		xPos += _tft->drawStringGFX("PCM ", xPos, 24, Open_Sans_Light_16);
	}

	/*std::vector<BarItem> barItems;
	for (int i = 0; i < 10; i++) {
		BarItem item(i, 10);
		barItems.push_back(item);
	}*/
	std::vector<BarItem2> barItems;
	for (int i = 0; i < 20; i++) {
		BarItem2 item(i, 20);
		barItems.push_back(item);
	}

	SlidingBar sb(26, 90, 110, 4);

	/*std::vector<RingSegmentItem> ringItems;
	for (int i = 0; i < 13; i++) {
		RingSegmentItem item(i, 13);
		ringItems.push_back(item);
	}*/

	if (false) {
		// Ring indicator demo
		std::vector<int> levels = { 99, 27, 0, 27, 48, 33, 88, 56, 2, 45, 99 };
		std::vector<int>::const_iterator levelIter = levels.cbegin();
		while (levelIter != levels.cend()) {
			/*std::vector<RingSegmentItem>::iterator iter = ringItems.begin();
			while (iter != ringItems.end()) {
				(*iter).update(*levelIter);
				++iter;
			}*/
			sb.update(*levelIter);
			Sleep(500);
			++levelIter;
		}
	}

	int xPos = 48;
	int yPos = 34;
	int prevX1 = -1;
	int prevX2 = -1;

	if (false) {
		// Clock demo
		int lastH1 = -1;
		int lastH2 = -1;
		int lastM1 = -1;
		int lastM2 = -1;

		//_tft->drawChar(48 + 10, 70, 0, ST7735S_FONT64_OSR); // ':'

		for (int h = 0; h < 24; h++) {
			int h1 = h / 10;
			if (h1 != lastH1) {
				//_tft->drawChar(48 + h1, 2, 5, ST7735S_FONT64_OSR);
				lastH1 = h1;
			}
			int h2 = h % 10;
			if (h2 != lastH2) {
				//_tft->drawChar(48 + h2, 36, 5, ST7735S_FONT64_OSR);
				lastH2 = h2;
			}
			for (int m = 0; m < 60; m++) {
				int m1 = m / 10;
				if (m1 != lastM1) {
					//_tft->drawChar(48 + m1, 86, 5, ST7735S_FONT64_OSR);
					lastM1 = m1;
				}
				int m2 = m % 10;
				if (m2 != lastM2) {
					//_tft->drawChar(48 + m2, 122, 5, ST7735S_FONT64_OSR);
					lastM2 = m2;
				}
				Sleep(100);
			}
		}
	}
	
	Label digit1(24, 90, ST7735S_WHITE, ST7735S_BLACK, Orbitron_Medium_72);
	Label digit2(84, 90, ST7735S_WHITE, ST7735S_BLACK, Orbitron_Medium_72);

	if (true) {
		//_text_color = ST7735S_WHITE;
		xPos = 76;
		yPos = 78;
		for (int i = 99; i >= 0; i -= 1) {
			digit1.setChar((i / 10) + 48);
			digit2.setChar((i % 10) + 48);
	
			//sb.update(i);

			std::vector<BarItem2>::iterator iter = barItems.begin();
			while (iter != barItems.end()) {
				//(*iter).update(i);
				++iter;
			}

			Sleep(100);
		}
	}
}

int __cdecl main(void)
{
	_tft = new ST7735S();
	printf("\n\n\n\n\n\n\n\n\n\n");
	draw();
	getchar();
	return 0;

	puts("GEVOL 3.0.0");
	puts("Print any key to exit...");

	bool b = Parameters::instance()->auto_find;
	VolumeControl volume(Parameters::instance()->getStartLevel());

	_http = new HttpServer();

	Spdif::StatusS msg;
	msg.input = Spdif::Coax1;
	msg.pcm_info = Spdif::PCM_NORMAL;
	msg.sample_rate = Spdif::SR_44100;	
	Parameters::instance()->setSpdifStatus(SpdifHelper::serializeStatusMsg(msg));

	msg.input = Spdif::Coax2;
	msg.pcm_info = Spdif::PCM_NORMAL;
	msg.sample_rate = Spdif::SR_192000;
	Parameters::instance()->setSpdifStatus(SpdifHelper::serializeStatusMsg(msg));

	msg.input = Spdif::Opt1;
	msg.pcm_info = Spdif::PCM_DD_AC3;
	msg.sample_rate = Spdif::SR_48000;
	Parameters::instance()->setSpdifStatus(SpdifHelper::serializeStatusMsg(msg));

	Parameters::instance()->current_level = 26;
	Parameters::instance()->current_input = Spdif::Coax1;
	Parameters::instance()->auto_find = true;

	std::thread t(threadFunc);

	printf("TCP server started!\n\n");
	while (!kbHit()) {
		Sleep(10);	
	}
	EventQueue::instance()->post(EVENT_SYSTEM_SHUTDOWN);
	t.join();

	delete _http;
	return 0;
}
