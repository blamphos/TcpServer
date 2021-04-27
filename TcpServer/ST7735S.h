#pragma once

#include <stdint.h>
#include <cmath>
#include <Windows.h>


const float DEG2RAD = 0.0174532925;

class ST7735S
{
public:
	ST7735S()
	{
		_myconsole = GetConsoleWindow();
		_dc = GetDC(_myconsole);
	}

	~ST7735S()
	{
		ReleaseDC(_myconsole, _dc);
	}

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
		SetPixel(_dc, x, y, c);
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

	void drawBitmap(const char* p_bitmap, int p_x, int p_y, int p_width, int p_height)
	{
		char line = 0;
		int pX = 0;
		int pY = p_y;
		int w = (p_width + 31) / 32 * 4; // bytes per row

		for (int i = 0; i < p_height; i++) {
			drawFastHLine(p_x, pY, p_width, _textBgcolor);
			for (int k = 0; k < w; k++) {
				line = *(p_bitmap + (w * i) + k);
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

	int drawChar(char p_char, uint16_t p_x, uint16_t p_y, GFXfont p_font, 
		uint16_t p_xOffset = 0, uint16_t p_clearWidth = 0)
	{
		int index = p_char - 32;

		if ((p_char < p_font.first) || (p_char > p_font.last)) {
			index = 0;
		}

		GFXglyph glyph = p_font.glyph[index];
		int pX = 0;
		int pY = p_y + glyph.yOffset;
		int w = 0;
		int len = ((glyph.width * glyph.height) + 7) / 8;
		uint8_t line = 0;
		int clearWidth = glyph.width + glyph.xOffset;
		if (p_clearWidth > 0) {
			clearWidth = p_clearWidth;
		}

		drawFastHLine(p_x, pY, clearWidth, _textBgcolor);

		for (int i = 0; i < len; i++) {
			line = (p_font.bitmap[glyph.bitmapOffset + i]);
			for (int x = 0; x < 8; x++) {
				pX = p_x + glyph.xOffset + p_xOffset + w;
				if ((line << x) & 0x80) {
					drawPixel(pX, pY, _text_color);
				}

				if (++w == glyph.width) {
					w = 0;
					pY++;
					drawFastHLine(p_x, pY, clearWidth, _textBgcolor);
				}
			}
		}

		return glyph.xAdvance;
	}

	int drawString(const char* p_str, uint16_t p_oX, uint16_t p_oY, GFXfont p_font)
	{
		int sumX = 0;

		//_cs->write(0);
		while (*p_str) {
			int xPlus = drawChar(*p_str++, p_oX, p_oY, p_font);
			sumX += xPlus;
			p_oX += xPlus; /* Move cursor right       */
		}
		//_cs->write(1);
		return sumX;
	}

	void setTextColor(uint16_t p_color)
	{
		_text_color = p_color;
	}

private:
	HDC _dc;
	HWND _myconsole;
	COLORREF COLOR_BLACK = RGB(0, 0, 0);
	COLORREF COLOR_CYAN = RGB(0, 255, 255);
	COLORREF COLOR_WHITE = RGB(255, 255, 255);
	COLORREF COLOR_DARKGREY = RGB(0x53, 0x53, 0x53);

	int _text_size = 1;
	int _text_color = ST7735S_WHITE;
	int _textBgcolor = ST7735S_BLACK;

	// Swap any type
	template <typename T> 
	static inline void tftswap(T& a, T& b) { T t = a; a = b; b = t; }
};