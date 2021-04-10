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
#include "Font16.h"
#include "Font32.h"
#include "Font64.h"
#include "Font56_OpenSansRegular.h"
#include "Font64_OpenSansRegular.h"
#include "Roboto_Mono_16_GFX.h"
#include "Roboto_Mono_32_GFX.h"
#include "Roboto_Mono_64_GFX.h"
#include "RobotoMonoMedium_12_GFX.h"
#include "RobotoMonoMedium_16_GFX.h"
#include "Orbitron_64_GFX.h"
#include "OpenSans_16_GFX.h"
#include "OpenSans_24_GFX.h"
#include "OpenSans_32_GFX.h"
#include "OpenSans_64_GFX.h"
#include "OpenSansLight_12_GFX.h"
#include "OpenSansLight_16_GFX.h"

HttpServer* _http;

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

void analyzeIr(IrCommandT p_irCommand)
{
	static std::vector<uint32_t> currentCodes = { 0x0, 0x0, 0x0 };
	static std::map<IrCommandT, std::vector<uint32_t>> recordedIrCodesMap;

	IrCommandT _irCommand = p_irCommand;
	std::vector<uint32_t> upCodes = {0x018484B4, 0x8484B4B0, 0x84B4B0B0, 0xB4B0B084, 0xB0B08434, 0xB0843434, 0x84343484, 0x343484B0, 0x3484B030};
	std::vector<uint32_t> downCodes = {0x018484B4, 0x8484B4B0, 0x84B4B0B0, 0xB4B0B0B0, 0xB0B0B034, 0xB0B034B4, 0xB034B430, 0x34B430B0, 0xB430B030};
	std::vector<uint32_t> muteCodes = {0x018484B4, 0x8484B4B0, 0x84B4B0B0, 0xB4B0B084, 0xB0B084B0, 0xB084B034, 0x84B03484, 0xB03484B0, 0x3484B030};
	/*std::vector<int> bytes = IrAnalyzerUtils::analyze(buff, BUFF_SIZE);
	commands.push_back(bytes);
	IrAnalyzerUtils::logBytes(bytes);

	IrAnalyzerUtils::trimStart(commands);
	IrAnalyzerUtils::logMatrix(commands);

	std::vector<uint32_t> codes;
	IrAnalyzerUtils::getCodes(commands.front(), codes);*/
	
	recordedIrCodesMap[IR_VOLUME_UP] = upCodes;
	recordedIrCodesMap[IR_VOLUME_DOWN] = downCodes;
	recordedIrCodesMap[IR_VOLUME_MUTE] = muteCodes;
	IrAnalyzerUtils::logIrCodeMap(recordedIrCodesMap);

	std::map<IrCommandT, std::vector<uint32_t>> uniqueIrCodesMap;
	IrAnalyzerUtils::getUniqueIrCodesMap(uniqueIrCodesMap, recordedIrCodesMap);
	IrAnalyzerUtils::logIrCodeMap(uniqueIrCodesMap);

	uint32_t irCode = IrAnalyzerUtils::getIrBinCode(uniqueIrCodesMap[_irCommand]);
	if (irCode != 0x0) {
		//std::vector<uint32_t> currentCodes = Parameters::instance()->getIrRemoteCodes();
		currentCodes[_irCommand] = irCode;
		//printf("Code: %08X\n", irCode);
		IrAnalyzerUtils::logIrCodes(currentCodes);
		//Parameters::instance()->setIrRemoteCodesList(currentCodes);
	}
	else {
		perror("IR learning failed");
	}

}

void drawCharRle(char* p_array, int p_len, int p_width)
{
	int w = 0;

	for (int i = 0; i < p_len; i++) {
		int c = static_cast<int>(p_array[i] & 0x7F) + 1;
		for (int x = 0; x < c; x++) {
			if (p_array[i] & 0x80) {
				printf("..");
			}
			else {
				printf("  ");
			}
			w++;
			if (w == p_width) {
				w = 0;
				printf("\n");
			}
		}
	}
}

HDC dc;
COLORREF COLOR_BLACK = RGB(0, 0, 0); 
COLORREF COLOR_CYAN = RGB(0, 255, 255);
COLORREF COLOR_WHITE = RGB(255, 255, 255);
COLORREF COLOR_DARKGREY = RGB(0x53, 0x53, 0x53);

int _text_size = 1;
int _text_color = ST7735S_WHITE;
int _text_bgcolor = ST7735S_BLACK;

const float DEG2RAD = 0.0174532925;

// Swap any type
template <typename T> static inline void
tftswap(T& a, T& b) { T t = a; a = b; b = t; }

void drawPixel(int16_t x, int16_t y, uint16_t color)
{
	COLORREF c = COLOR_BLACK;

	switch (color) {
	case ST7735S_BLACK:
		break;
	case ST7735S_WHITE:
		c = COLOR_WHITE;
		break;
	case ST7735S_CYAN:
		c = COLOR_CYAN;
		break;
	case ST7735S_DARKGREY:
		c = COLOR_DARKGREY;
		break;
	default:
		break;
	}
	SetPixel(dc, x, y, c);
	/*if ((x >= _width) || (y >= _height)) {
		return;
	}

	setAddrWindow(x, y, x + 1, y + 1);
	writeWord(color);*/
}

void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	while (h--) {
		drawPixel(x, y++, color);
		//writeWord(color);
	}
}

void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	while (w--) {
		drawPixel(x++, y, color);
		//writeWord(color);
	}
}

void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	//_cs->write(0);
	drawFastHLine(x, y, w, color);
	drawFastHLine(x, y + h - 1, w, color);
	drawFastVLine(x, y, h, color);
	drawFastVLine(x + w - 1, y, h, color);
	//_cs->write(1);
}

int drawChar(int p_char, int p_x, int p_y, int p_font)
{
	if (p_font != 0) {
		p_char -= 32;
	}

	char width = 0;
	int height = 0;
	const char* flash_address = 0;
	char gap = 0;
	int startPixel = 0;

	if (p_font == ST7735S_FONT16) {
		flash_address = chrtbl_f16[p_char];
		width = widtbl_f16[p_char];
		height = F16_CHAR_HEIGHT;
		gap = 1;
	}
	else if (p_font == ST7735S_FONT32) {
		flash_address = chrtbl_f32[p_char];
		width = widtbl_f32[p_char];
		height = F32_CHAR_HEIGHT;
		gap = 0;
	}
	else if (p_font == ST7735S_FONT64) {
		flash_address = chrtbl_f64[p_char];
		width = widtbl_f64[p_char];
		height = F64_CHAR_HEIGHT;
		gap = 0;
	}
	else if (p_font == ST7735S_FONT56_OSR) {
		flash_address = chrtbl_f56_osr[p_char];
		width = widtbl_f56_osr[p_char];
		height = F56_OSR_CHAR_HEIGHT;
		gap = 0;
		startPixel = 72;
	}
	else if (p_font == ST7735S_FONT64_OSR) {
		flash_address = chrtbl_f64_osr[p_char];
		width = widtbl_f64_osr[p_char];
		height = F64_OSR_CHAR_HEIGHT;
		gap = 0;
		startPixel = 74;
	}
	/*else if (p_font == ST7735S_FONT72) {
		flash_address = chrtbl_f72[p_char];
		width = widtbl_f72[p_char];
		height = F72_CHAR_HEIGHT;
		gap = 0;
	}*/
	else {
		return 0;
	}

	int w = (width + 7) / 8;
	int pX = 0;
	int pY = p_y;
	char line = 0;

	if ((p_font == ST7735S_FONT56_OSR) || (p_font == ST7735S_FONT64_OSR)) {
		w = 0;
		int len = width * height / 8 + 1;
		drawFastHLine(p_x, pY, width + gap, _text_bgcolor);

		for (int i = startPixel; i < len; i++) {
			line = *(flash_address + i);
			for (int x = 0; x < 8; x++) {
				pX = p_x + w;
				if ((line >> x) & 0x1) {
					drawPixel(pX, pY, _text_color);
				}

				if (++w == width) {
					w = 0;
					pY++;
					drawFastHLine(p_x, pY, width + gap, _text_bgcolor);
				}
			}
		}
	}
	else {
		for (int i = 0; i < height; i++) {
			if (_text_color != _text_bgcolor) {
				if (_text_size == 1) {
					drawFastHLine(p_x, pY, width + gap, _text_bgcolor);
				}
				/*else {
					fillRect(p_x, pY, (width + gap) * _text_size, _text_size, _text_bgcolor);
				}*/
			}

			for (int k = 0; k < w; k++) {
				line = *(flash_address + (w * i) + k);

				if (line != 0) {
					if (_text_size == 1) {
						pX = p_x + k * 8;
						if (line & 0x80) {
							drawPixel(pX, pY, _text_color);
						}
						if (line & 0x40) {
							drawPixel(pX + 1, pY, _text_color);
						}
						if (line & 0x20) {
							drawPixel(pX + 2, pY, _text_color);
						}
						if (line & 0x10) {
							drawPixel(pX + 3, pY, _text_color);
						}
						if (line & 0x8) {
							drawPixel(pX + 4, pY, _text_color);
						}
						if (line & 0x4) {
							drawPixel(pX + 5, pY, _text_color);
						}
						if (line & 0x2) {
							drawPixel(pX + 6, pY, _text_color);
						}
						if (line & 0x1) {
							drawPixel(pX + 7, pY, _text_color);
						}
					}
					/*else {
						pX = p_x + k * 8 * _text_size;
						if (line & 0x80) {
							fillRect(pX, pY, _text_size, _text_size, _text_color);
						}
						if (line & 0x40) {
							fillRect(pX + _text_size, pY, _text_size, _text_size, _text_color);
						}
						if (line & 0x20) {
							fillRect(pX + 2 * _text_size, pY, _text_size, _text_size, _text_color);
						}
						if (line & 0x10) {
							fillRect(pX + 3 * _text_size, pY, _text_size, _text_size, _text_color);
						}
						if (line & 0x8) {
							fillRect(pX + 4 * _text_size, pY, _text_size, _text_size, _text_color);
						}
						if (line & 0x4) {
							fillRect(pX + 5 * _text_size, pY, _text_size, _text_size, _text_color);
						}
						if (line & 0x2) {
							fillRect(pX + 6 * _text_size, pY, _text_size, _text_size, _text_color);
						}
						if (line & 0x1) {
							fillRect(pX + 7 * _text_size, pY, _text_size, _text_size, _text_color);
						}
					}*/
				}
			}
			pY += _text_size;
		}
	}

	return (width + gap) * _text_size;
}

int drawString(const char* str, int poX, int poY, int font)
{
	int sumX = 0;

	//_cs->write(0);
	while (*str) {
		int xPlus = drawChar(*str++, poX, poY, font);
		sumX += xPlus;
		poX += xPlus; /* Move cursor right       */
	}
	//_cs->write(1);
	return sumX;
}

void drawChar2(char* p_array, int p_height, int p_width)
{
	int wc = 0;
	int w = (p_width + 7) / 8;
	char line = 0;

	for (int i = 0; i < p_height; i++) {
		for (int k = 0; k < w; k++) {
			line = *(p_array + (w * i) + k);
			for (int x = 0; x < 8; x++) {
				//int c = static_cast<int>(line) >> x;
				if ((line >> x) & 0x1) {
					printf("..");
				}
				else {
					printf("  ");
				}
				wc++;
				if (wc == p_width) {
					wc = 0;
					printf("\n");
				}
			}
		}
	}
}

void convertRleToBitmap(char* p_array, int p_len, int p_width)
{
	int w = 0;
	int b = 0;
	int bc = 0;

	for (int i = 0; i < p_len; i++) {
		for (int x = 0; x < 8; x++) {
			//int c = static_cast<int>(p_array[i]) << x;
			//if (c & 0x80) {
			int c = static_cast<int>(p_array[i]) >> x;
			if (c & 0x1) {
				b |= (1 << (7 - x));
			}
			else {
				
			}
			w++;
			bc++;
			if (w == p_width) {
				w = 0;
				bc++;
			}

			if (bc == 8) {
				printf("0x%02X,", b);
				bc = 0;
				b = 0;
			}
		}
	}
}

void drawCharRpi(char* p_array, int p_len, int p_width, int p_height)
{
	const int _text_size = 1;
	int y = 0;
	int x = 0;
	int width = p_width;
	int height = p_height;
	char* flash_address = p_array;

	int w = (width + 7) / 8;
	int pX = 0;
	int pY = y;
	char line = 0;

	for (int i = 0; i < height; i++) {
		/*if (_text_color != _text_bgcolor) {
			if (_text_size == 1) {
				drawFastHLine(x, pY, width + gap, _text_bgcolor);
			}
			else {
				fillRect(x, pY, (width + gap) * _text_size, _text_size, _text_bgcolor);
			}
		}*/
		for (int k = 0; k < w; k++) {
			line = *(flash_address + (w * i) + k);

			if (line != 0) {
				if (_text_size == 1) {
					pX = x + k * 8;
					for (int s = 0; s < 8; s++) {
						if ((line << s) & 0x80) {
							printf("..");
						}
						else {
							printf("  ");
						}
					}
				}
			}
			else {
				for (int c = 0; c < 8; c++) {
					printf("  ");
				}
			}
		}
		pY += _text_size;
		//printf("\n");
	}
}

void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -r - r;
	int16_t x = 0;

	//_cs->write(0);
	drawPixel(x0 + r, y0, color);
	drawPixel(x0 - r, y0, color);
	drawPixel(x0, y0 - r, color);
	drawPixel(x0, y0 + r, color);

	while (x < r) {
		if (f >= 0) {
			r--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		drawPixel(x0 + x, y0 + r, color);
		drawPixel(x0 - x, y0 + r, color);
		drawPixel(x0 - x, y0 - r, color);
		drawPixel(x0 + x, y0 - r, color);

		drawPixel(x0 + r, y0 + x, color);
		drawPixel(x0 - r, y0 + x, color);
		drawPixel(x0 - r, y0 - x, color);
		drawPixel(x0 + r, y0 - x, color);
	}
	//_cs->write(1);
}

void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;

	//_cs->write(0);
	while (x < r) {
		if (f >= 0) {
			r--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x8) {
			drawPixel(x0 - r, y0 + x, color);
			drawPixel(x0 - x, y0 + r, color);
		}
		if (cornername & 0x4) {
			drawPixel(x0 + x, y0 + r, color);
			drawPixel(x0 + r, y0 + x, color);
		}
		if (cornername & 0x2) {
			drawPixel(x0 + r, y0 - x, color);
			drawPixel(x0 + x, y0 - r, color);
		}
		if (cornername & 0x1) {
			drawPixel(x0 - x, y0 - r, color);
			drawPixel(x0 - r, y0 - x, color);
		}
	}
	//_cs->write(1);
}

int _height = 132;
int _width = 162;

void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	//spi_begin();

	int8_t steep = abs(y1 - y0) > abs(x1 - x0);

	if (steep) {
		tftswap(x0, y0);
		tftswap(x1, y1);
	}

	if (x0 > x1) {
		tftswap(x0, x1);
		tftswap(y0, y1);
	}

	if (x1 < 0) return;

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int8_t ystep = (y0 < y1) ? 1 : (-1);

	if (steep)	// y increments every iteration (y0 is x-axis, and x0 is y-axis)
	{
		if (x1 >= _height) x1 = _height - 1;

		for (; x0 <= x1; x0++) {
			if ((x0 >= 0) && (y0 >= 0) && (y0 < _width)) break;
			err -= dy;
			if (err < 0) {
				err += dx;
				y0 += ystep;
			}
		}

		if (x0 > x1) return;

		//setWindow(y0, x0, y0, _height);
		for (; x0 <= x1; x0++) {
			//spiWrite16s(color);
			err -= dy;
			if (err < 0) {
				y0 += ystep;
				if ((y0 < 0) || (y0 >= _width)) break;
				err += dx;
				//while (!(SPSR & _BV(SPIF))); // Safe, but can comment out and rely on delay
				//setWindow(y0, x0 + 1, y0, _height);
			}
		}
	}
	else	// x increments every iteration (x0 is x-axis, and y0 is y-axis)
	{
		if (x1 >= _width) x1 = _width - 1;

		for (; x0 <= x1; x0++) {
			if ((x0 >= 0) && (y0 >= 0) && (y0 < _height)) break;
			err -= dy;
			if (err < 0) {
				err += dx;
				y0 += ystep;
			}
		}

		if (x0 > x1) return;

		//setWindow(x0, y0, _width, y0);
		for (; x0 <= x1; x0++) {
			//spiWrite16s(color);
			err -= dy;
			if (err < 0) {
				y0 += ystep;
				if ((y0 < 0) || (y0 >= _height)) break;
				err += dx;
				//while (!(SPSR & _BV(SPIF))); // Safe, but can comment out and rely on delay
				//setWindow(x0 + 1, y0, _width, y0);
			}
		}
	}
	//TFT_CS_H;
	//spi_end();
}

void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	drawLine(x0, y0, x1, y1, color);
	drawLine(x1, y1, x2, y2, color);
	drawLine(x2, y2, x0, y0, color);
}

void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{
	int16_t a, b, y, last;

	// Sort coordinates by Y order (y2 >= y1 >= y0)
	if (y0 > y1) {
		tftswap(y0, y1); tftswap(x0, x1);
	}
	if (y1 > y2) {
		tftswap(y2, y1); tftswap(x2, x1);
	}
	if (y0 > y1) {
		tftswap(y0, y1); tftswap(x0, x1);
	}

	if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
		a = b = x0;
		if (x1 < a)      a = x1;
		else if (x1 > b) b = x1;
		if (x2 < a)      a = x2;
		else if (x2 > b) b = x2;
		drawFastHLine(a, y0, b - a + 1, color);
		return;
	}

	int16_t
		dx01 = x1 - x0,
		dy01 = y1 - y0,
		dx02 = x2 - x0,
		dy02 = y2 - y0,
		dx12 = x2 - x1,
		dy12 = y2 - y1,
		sa = 0,
		sb = 0;

	// For upper part of triangle, find scanline crossings for segments
	// 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
	// is included here (and second loop will be skipped, avoiding a /0
	// error there), otherwise scanline y1 is skipped here and handled
	// in the second loop...which also avoids a /0 error here if y0=y1
	// (flat-topped triangle).
	if (y1 == y2) last = y1;  // Include y1 scanline
	else         last = y1 - 1; // Skip it

	for (y = y0; y <= last; y++) {
		a = x0 + sa / dy01;
		b = x0 + sb / dy02;
		sa += dx01;
		sb += dx02;

		if (a > b) tftswap(a, b);
		drawFastHLine(a, y, b - a + 1, color);
	}

	// For lower part of triangle, find scanline crossings for segments
	// 0-2 and 1-2.  This loop is skipped if y1=y2.
	sa = dx12 * (y - y1);
	sb = dx02 * (y - y0);
	for (; y <= y2; y++) {
		a = x1 + sa / dy12;
		b = x0 + sb / dy02;
		sa += dx12;
		sb += dx02;

		if (a > b) tftswap(a, b);
		drawFastHLine(a, y, b - a + 1, color);
	}
}

// #########################################################################
// Draw an arc with a defined thickness
// #########################################################################

// x,y == coords of centre of arc
// start_angle = 0 - 359
// seg_count = number of 3 degree segments to draw (120 => 360 degree arc)
// rx = x axis radius
// yx = y axis radius
// w  = width (thickness) of arc in pixels
// colour = 16 bit colour value
// Note if rx and ry are the same an arc of a circle is drawn

void fillArc(int16_t x, int16_t y, int16_t start_angle, int16_t seg_count, 
	int16_t rx, int16_t ry, int16_t w, uint16_t color)
{
	int seg = 6; // Segments are 3 degrees wide = 120 segments for 360 degrees
	int inc = 6; // Draw segments every 3 degrees, increase to 6 for segmented ring

	// Draw colour blocks every inc degrees
	for (int i = start_angle; i < start_angle + seg * seg_count; i += inc) {
		// Calculate pair of coordinates for segment start
		float sx = cos((i - 90) * DEG2RAD);
		float sy = sin((i - 90) * DEG2RAD);
		uint16_t x0 = sx * (rx - w) + x;
		uint16_t y0 = sy * (ry - w) + y;
		uint16_t x1 = sx * rx + x;
		uint16_t y1 = sy * ry + y;

		// Calculate pair of coordinates for segment end
		float sx2 = cos((i + seg - 90) * DEG2RAD);
		float sy2 = sin((i + seg - 90) * DEG2RAD);
		int x2 = sx2 * (rx - w) + x;
		int y2 = sy2 * (ry - w) + y;
		int x3 = sx2 * rx + x;
		int y3 = sy2 * ry + y;

		fillTriangle(x0, y0, x1, y1, x2, y2, color);
		fillTriangle(x1, y1, x2, y2, x3, y3, color);
	}
}

void fillRect(int16_t p_x, int16_t p_y, int16_t p_w, int16_t p_h, uint16_t p_color)
{
	// Rudimentary clipping (drawChar w/big text requires this)
	/*if ((x >= _width) || (y >= _height)) {
		return;
	}

	if ((x + w - 1) >= _width) {
		w = _width - x;
	}

	if ((y + h - 1) >= _height) {
		h = _height - y;
	}*/

	//_cs->write(0);
	//setAddrWindow(x, y, x + w - 1, y + h - 1);

	for (int y = 0; y < p_h; y++) {
		for (int x = 0; x < p_w; x++) {
			drawPixel(p_x + x, p_y + y, p_color);
		}
	}
	//_cs->write(1);
}

/***************************************************************************************
** Function name:           fillCircleHelper
** Description:             Support function for filled circle drawing
***************************************************************************************/
// Used to do circles and roundrects
void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -r - r;
	int16_t x = 0;

	delta++;
	while (x < r) {
		if (f >= 0) {
			r--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1) {
			drawFastVLine(x0 + x, y0 - r, r + r + delta, color);
			drawFastVLine(x0 + r, y0 - x, x + x + delta, color);
		}
		if (cornername & 0x2) {
			drawFastVLine(x0 - x, y0 - r, r + r + delta, color);
			drawFastVLine(x0 - r, y0 - x, x + x + delta, color);
		}
	}
}

/***************************************************************************************
** Function name:           fillCircle
** Description:             draw a filled circle
***************************************************************************************/
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	drawFastVLine(x0, y0 - r, r + r + 1, color);
	fillCircleHelper(x0, y0, r, 3, 0, color);
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
		
		fillCircle(_x, _y, _size, ST7735S_DARKGREY);
	}

	void update(int p_level)
	{
		if (p_level >= _min) {
			fillCircle(_x, _y, _size, ST7735S_WHITE);
		}
		else {
			fillCircle(_x, _y, _size, ST7735S_DARKGREY);
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
		//drawRect(_x, _y, 12, _h, ST7735S_DARKGREY);
	}

	void update(int p_level)
	{
		if (p_level && p_level >= _min) {
			fillRect(_x, _y, 12, _h, ST7735S_WHITE);
		}
		else {
			fillRect(_x, _y, 12, _h, ST7735S_BLACK);
			drawRect(_x, _y, 12, _h, ST7735S_DARKGREY);
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
				fillRect(_x, _y, _w, _h, ST7735S_WHITE);
				//drawRect(_x, _y, 14, _h, ST7735S_DARKGREY);
				_dimmed = false;
			}
		}
		else {
			if (!_dimmed) {
				fillRect(_x, _y, _w, _h, ST7735S_DARKGREY);
				//drawRect(_x, _y, _w, _h, ST7735S_DARKGREY);
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

void drawBitmap(const char* p_bitmap, int p_x, int p_y, int p_width, int p_height)
{
	char line = 0;
	int pX = 0;
	int pY = p_y;
	int w = (p_width + 7) / 8;

	for (int i = 0; i < p_height; i++) {
		drawFastHLine(p_x, pY, p_width, _text_bgcolor);
		for (int k = 0; k < w; k++) {
			line = *(p_bitmap + (4 * i) + k);
			if (line != 0) {
				pX = p_x + k * 8;
				for (int x = 0; x < 8; x++) {
					if ((line << x) & 0x80) {
						drawPixel(pX + x, pY, _text_color);
					}
				}
			}
		}
		pY++;
	}
}

int drawCharGFX(char p_char, uint16_t p_x, uint16_t p_y, GFXfont p_font)
{
	int index = p_char - 32;

	if ((p_char < p_font.first) || (p_char > p_font.last)) {
		/*int maxWidth = 0;
		for (int i = p_font.first; i < p_font.last; i++) {
			int w = p_font.glyph[i - 32].width;
			if (w > maxWidth) {
				maxWidth = w;
			}
			//printf("%d, ", p_font.glyph[i - 32].width);
		}
		fillRect(p_x, p_y, maxWidth, p_font.yAdvance, ST7735S_BLACK);
		return 0;*/
		index = 0;
	}

	GFXglyph glyph = p_font.glyph[index];

	int pX = 0;
	int pY = p_y + glyph.yOffset;
	int w = 0;
	int len = ((glyph.width * glyph.height) + 7) / 8;
	uint8_t line = 0;

	//drawFastHLine(p_x, pY, glyph.width + glyph.xOffset, _text_bgcolor);

	for (int i = 0; i < len; i++) {
		line = (p_font.bitmap[glyph.bitmapOffset + i]);
		for (int x = 0; x < 8; x++) {
			pX = p_x + glyph.xOffset + w;
			if ((line << x) & 0x80) {
				drawPixel(pX, pY, _text_color);
			}

			if (++w == glyph.width) {
				w = 0;
				pY++;
				//drawFastHLine(p_x, pY, glyph.width + glyph.xOffset, _text_bgcolor);
			}
		}
	}

	return glyph.xAdvance;
}

int drawStringGFX(const char* p_str, uint16_t p_oX, uint16_t p_oY, GFXfont p_font)
{
	int sumX = 0;

	//_cs->write(0);
	while (*p_str) {
		int xPlus = drawCharGFX(*p_str++, p_oX, p_oY, p_font);
		sumX += xPlus;
		p_oX += xPlus; /* Move cursor right       */
	}
	//_cs->write(1);
	return sumX;
}

class Label {
public:
	Label(int p_x, int p_y, uint16_t p_color, uint16_t p_backColor, GFXfont p_font) :
		_x(p_x), _y(p_y), _width(0), _height(0), _color(p_color), _backColor(p_backColor), _font(p_font), _prevChar(0)
	{
		// Scan glyphs to set the (max) height
		for (int i = 0; i < (_font.last - _font.first); i++) {
			if (_font.glyph[i].height > _height) {
				_height = _font.glyph[i].height;
			}

			switch (static_cast<char>(i + 32))
			{
			case '0': case '1': case '2': case '3': case '4': 
			case '5': case '6': case '7': case '8': case '9':
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
			fillRect(_x, _y - _height, _width, _height, _backColor);
		}

		_width = drawStringGFX(p_str, _x, _y, _font);
	}

	void setChar(int p_char)
	{
		if (p_char == _prevChar) {
			return;
		}
		_prevChar = p_char;

		if (_width > 0) {
			fillRect(_x, _y - _height, _width, _height, _backColor);
		}

		char c = static_cast<char>(p_char);
		int xOffset = (_width - _font.glyph[p_char - 32].xAdvance) / 2;
		drawCharGFX(c, _x + xOffset, _y, _font);
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
		/*
		int xOffset = 26;
		int w = 162 - (xOffset * 2);
		int yOffset = 14;
		int onWidth = w * i / 99;
		int offWidth = w * (99 - i) / 99;
		fillRect(xOffset, yPos + yOffset + 1, onWidth, 2, ST7735S_WHITE);
		fillRect(xOffset + onWidth, yPos + yOffset + 1, offWidth, 2, ST7735S_BLACK);
		//drawRect(xOffset + onWidth, yPos + yOffset, offWidth, 3, ST7735S_DARKGREY);
		drawRect(xOffset, yPos + yOffset, w, 4, ST7735S_DARKGREY);
		//fillRect(30 + i, yPos + 10, 99 - i, 1, ST7735S_DARKGREY);
		*/
	}

	void update(int p_level)
	{
		if (p_level == _lastLevel) {
			return;
		}

		if (_lastLevel == 0) {
			//fillRect(xOffset, yPos + yOffset + 1, onWidth, 2, ST7735S_WHITE);
			//fillRect(xOffset + onWidth, yPos + yOffset + 1, offWidth, 2, ST7735S_BLACK);
			//drawRect(xOffset + onWidth, yPos + yOffset, offWidth, 3, ST7735S_DARKGREY);
			drawRect(_x0, _y0, _width, _height, ST7735S_DARKGREY);
		}

		int barWidth = _width * p_level / 99;
		int diff = p_level - _lastLevel;

		if (diff >= 0) {
			fillRect(_x0, _y0, barWidth, _height, ST7735S_WHITE);
		}
		else {
			fillRect(_x0 + barWidth, _y0 + 1, _width - barWidth, _height - 2, ST7735S_BLACK);
			drawRect(_x0 + barWidth, _y0, _width - barWidth, _height, ST7735S_DARKGREY);
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
	HWND myconsole = GetConsoleWindow();
	dc = GetDC(myconsole);

	fillRect(0, 0, 162, 132, ST7735S_BLACK);
	drawRect(0, 0, 162, 132, ST7735S_WHITE);

	if (false) {
		int r = 20;
		for (int i = 0; i < 5; i++) {
			fillArc(81, 61, 60, 10, r, r, 2, ST7735S_WHITE);
			r += 5;
		}
		return;
	}

	if (false) {
		int x = 8;
		for (int i = 0; i < 10; i++) {
			//fillArc(81, 61, 60, 10, r, r, 2, ST7735S_WHITE);
			fillRect(x, 80, 12, 10, ST7735S_WHITE);
			x += 14;
		}
		return;
	}

	if (false) {
		int vx = 6;
		int vy = 134;
		//drawStringGFX("COAX2", vx, vy, Roboto_Mono_32);
		drawStringGFX("192kHz PCM 0123456789", 170, 10, Roboto_Mono_16);
		drawStringGFX("192kHz PCM 0123456789", 170, 30, Roboto_Mono_32);
		drawStringGFX("0123456789", 170, 70, Roboto_Mono_64);

		//drawStringGFX("192KHZ PCM 0123456789", 400, 140, Open_Sans_Regular_16);
		drawStringGFX("192 kHz PCM 0123456789", 400, 140, Open_Sans_Light_16);
		drawStringGFX("COAX2 0123456789", 400, 160, Open_Sans_Regular_24);
		drawStringGFX("COAX2 0123456789", 400, 190, Open_Sans_Regular_32);
		drawStringGFX("COAX2 0123456789", 400, 220, Open_Sans_Regular_64);
		drawStringGFX("0123456789", 400, 290, Orbitron_Medium_64);
	}
	//drawBitmap(lanIconBitmap24x20, 110, 6, 24, 20);
	drawBitmap(wifiIconBitmap24x24, 130, 3, 24, 20);

	if (false) {
		int xPos = 8;
		xPos += drawString("COAX2", xPos, 102, ST7735S_FONT32);
		if (Parameters::instance()->auto_find) {
			drawString("AUTO", 116, 108, ST7735S_FONT16);
		}

		xPos = 8;
		xPos += drawString("192kHz ", xPos, 6, ST7735S_FONT16);
		xPos += drawString("PCM ", xPos, 6, ST7735S_FONT16);
	}
	else {
		int xPos = 8;
		_text_color = ST7735S_CYAN;
		//xPos += drawString("COAX2", xPos, 102, ST7735S_FONT32);
		//drawStringGFX("COAX2", xPos, 126, Roboto_Mono_32);
		//drawStringGFX("COAX2", xPos, 122, Open_Sans_Regular_24);
		Label inputLabel(8, 122, ST7735S_CYAN, ST7735S_BLACK, Open_Sans_Regular_24);
		inputLabel.setText("COAX2");


		if (Parameters::instance()->auto_find) {
			//drawStringGFX("AUTO", 116, 122, Open_Sans_Light_12);
			Label autoLabel(116, 122, ST7735S_CYAN, ST7735S_BLACK, Open_Sans_Light_12);
			autoLabel.setText("AUTO");
			//drawRect(116, 102, 40, 20, ST7735S_CYAN);
		}

		xPos = 8;
		xPos += drawStringGFX("192 kHz ", xPos, 20, Open_Sans_Light_16);
		xPos += drawStringGFX("PCM ", xPos, 20, Open_Sans_Light_16);
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

		drawChar(48 + 10, 70, 0, ST7735S_FONT64_OSR); // ':'

		for (int h = 0; h < 24; h++) {
			int h1 = h / 10;
			if (h1 != lastH1) {
				drawChar(48 + h1, 2, 5, ST7735S_FONT64_OSR);
				lastH1 = h1;
			}
			int h2 = h % 10;
			if (h2 != lastH2) {
				drawChar(48 + h2, 36, 5, ST7735S_FONT64_OSR);
				lastH2 = h2;
			}
			for (int m = 0; m < 60; m++) {
				int m1 = m / 10;
				if (m1 != lastM1) {
					drawChar(48 + m1, 86, 5, ST7735S_FONT64_OSR);
					lastM1 = m1;
				}
				int m2 = m % 10;
				if (m2 != lastM2) {
					drawChar(48 + m2, 122, 5, ST7735S_FONT64_OSR);
					lastM2 = m2;
				}
				Sleep(100);
			}
		}
	}

	if (false) {
		for (int i = 0; i < 100; i += 1) {
			int x1 = i / 10;
			if (x1 != prevX1) {
				//drawChar(48 + x1, xPos, yPos, ST7735S_FONT56_OSR);
				drawChar(48 + x1, xPos, 10, ST7735S_FONT64_OSR);
				prevX1 = x1;
			}
			int x2 = i % 10;
			if (x2 != prevX2) {
				//drawChar(48 + x2, xPos + 34, yPos, ST7735S_FONT56_OSR);
				drawChar(48 + x2, xPos + 37, 10, ST7735S_FONT64_OSR);
				prevX2 = x2;
			}
			break;

			/*std::vector<RingSegmentItem>::iterator iter = ringItems.begin();
			while (iter != ringItems.end()) {
				(*iter).update(i);
				++iter;
			}*/

			Sleep(150);
		}
	}
	
	Label digit1(28, 86, ST7735S_WHITE, ST7735S_BLACK, Orbitron_Medium_64);
	Label digit2(82, 86, ST7735S_WHITE, ST7735S_BLACK, Orbitron_Medium_64);

	if (true) {
		_text_color = ST7735S_WHITE;
		xPos = 76;
		yPos = 78;
		for (int i = 99; i >= 0; i -= 1) {
			digit1.setChar((i / 10) + 48);
			digit2.setChar((i % 10) + 48);
	
			/*int x1 = i / 10;
			if (x1 != prevX1) {
				//drawChar(48 + x1, xPos, yPos, ST7735S_FONT64_OSR);
				
				//fillRect(xPos, yPos-47, 36, 48, ST7735S_BLACK);
				//drawCharGFX(48 + x1, xPos, yPos, Open_Sans_Regular_64);

				digit1.setChar(x1 + 48);
				//fillRect(xPos-24, yPos - 47, 52, 48, ST7735S_BLACK);
				//drawCharGFX(48 + x1, xPos-24, yPos, Orbitron_Medium_64);
				prevX1 = x1;
			}
			int x2 = i % 10;
			if (x2 != prevX2) {
				//drawChar(48 + x2, xPos + 37, yPos, ST7735S_FONT64_OSR);

				//fillRect(xPos + 40, yPos-47, 36, 48, ST7735S_BLACK);
				//drawCharGFX(48 + x2, xPos + 40, yPos, Open_Sans_Regular_64);

				digit2.setChar(x2 + 48);
				//fillRect(xPos + 30, yPos-47, 52, 48, ST7735S_BLACK);
				//drawCharGFX(48 + x2, xPos + 30, yPos, Orbitron_Medium_64);
				prevX2 = x2;
			}*/
			//break;
			
			/*std::vector<RingSegmentItem>::iterator iter = ringItems.begin();
			while (iter != ringItems.end()) {
				(*iter).update(i);
				++iter;
			}*/

			/*int xOffset = 26;
			int w = 162 - (xOffset * 2);
			int yOffset = 14;
			int onWidth = w * i / 99;
			int offWidth = w * (99 - i) / 99;
			fillRect(xOffset, yPos + yOffset + 1, onWidth, 2, ST7735S_WHITE);
			fillRect(xOffset + onWidth, yPos + yOffset + 1, offWidth, 2, ST7735S_BLACK);
			//drawRect(xOffset + onWidth, yPos + yOffset, offWidth, 3, ST7735S_DARKGREY);
			drawRect(xOffset, yPos + yOffset, w, 4, ST7735S_DARKGREY);
			//fillRect(30 + i, yPos + 10, 99 - i, 1, ST7735S_DARKGREY);*/

			/*std::vector<BarItem>::iterator iter = barItems.begin();
			while (iter != barItems.end()) {
				(*iter).update(i);
				++iter;
			}*/
			sb.update(i);

			std::vector<BarItem2>::iterator iter = barItems.begin();
			while (iter != barItems.end()) {
				//(*iter).update(i);
				++iter;
			}

			Sleep(100);
		}
	}

	ReleaseDC(myconsole, dc);
}

int __cdecl main(void)
{
	printf("\n\n\n\n\n\n\n\n\n\n");
	draw();
	getchar();
	return 0;

	//int s = sizeof(chr_f56_30) / sizeof(char);
	//drawChar(chr_f56_30, 56, 47);
	//drawChar(chr_f56_31, 56, 22);
	//convertRleToBitmap(chr_f56_30, s, 47);
	//drawCharRpi2(test, 0, 47, 55);
	//drawCharRpi(chr_f64_30, 0, 30, 48);
	getchar();

	analyzeIr(IR_VOLUME_UP);
	analyzeIr(IR_VOLUME_DOWN);
	analyzeIr(IR_VOLUME_MUTE);

	getchar();
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
