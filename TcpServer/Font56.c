// This Font size 8 is a Arial 70 pixel height font intended to display large numbers
// This font only contains characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .
// All other characters print as a space

#include "Font56.h"

const char widtbl_f56[96] =          // character width table
{
	0x0F, // 32: 15
	0x0F, // 33: 15
	0x16, // 34: 22
	0x24, // 35: 36
	0x20, // 36: 32
	0x2E, // 37: 46
	0x29, // 38: 41
	0x0C, // 39: 12
	0x11, // 40: 17
	0x11, // 41: 17
	0x1F, // 42: 31
	0x20, // 43: 32
	0x0E, // 44: 14
	0x12, // 45: 18
	0x0F, // 46: 15
	0x15, // 47: 21
	0x20, // 48: 32
	0x20, // 49: 32
	0x20, // 50: 32
	0x20, // 51: 32
	0x20, // 52: 32
	0x20, // 53: 32
	0x20, // 54: 32
	0x20, // 55: 32
	0x20, // 56: 32
	0x20, // 57: 32
	0x0F, // 58: 15
	0x0F, // 59: 15
	0x20, // 60: 32
	0x20, // 61: 32
	0x20, // 62: 32
	0x18, // 63: 24
	0x32, // 64: 50
	0x23, // 65: 35
	0x24, // 66: 36
	0x23, // 67: 35
	0x29, // 68: 41
	0x1F, // 69: 31
	0x1D, // 70: 29
	0x29, // 71: 41
	0x29, // 72: 41
	0x10, // 73: 16
	0x0F, // 74: 15
	0x22, // 75: 34
	0x1D, // 76: 29
	0x33, // 77: 51
	0x2A, // 78: 42
	0x2C, // 79: 44
	0x22, // 80: 34
	0x2C, // 81: 44
	0x23, // 82: 35
	0x1F, // 83: 31
	0x1F, // 84: 31
	0x29, // 85: 41
	0x21, // 86: 33
	0x34, // 87: 52
	0x20, // 88: 32
	0x1F, // 89: 31
	0x20, // 90: 32
	0x12, // 91: 18
	0x15, // 92: 21
	0x12, // 93: 18
	0x1E, // 94: 30
	0x19, // 95: 25
	0x20, // 96: 32
	0x1F, // 97: 31
	0x22, // 98: 34
	0x1B, // 99: 27
	0x22, // 100: 34
	0x1F, // 101: 31
	0x13, // 102: 19
	0x1F, // 103: 31
	0x22, // 104: 34
	0x0E, // 105: 14
	0x0E, // 106: 14
	0x1D, // 107: 29
	0x0E, // 108: 14
	0x34, // 109: 52
	0x22, // 110: 34
	0x22, // 111: 34
	0x22, // 112: 34
	0x22, // 113: 34
	0x17, // 114: 23
	0x1B, // 115: 27
	0x14, // 116: 20
	0x22, // 117: 34
	0x1C, // 118: 28
	0x2C, // 119: 44
	0x1D, // 120: 29
	0x1C, // 121: 28
	0x1A, // 122: 26
	0x15, // 123: 21
	0x1F, // 124: 31
	0x15, // 125: 21
	0x20, // 126: 32
	0x22, // 127: 34
};

// Row format, MSB left

const char chr_f56_20[] =          // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const char chr_f56_2E[] =          // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x07,0xC0,0x07,0xF0,0x07,0xF8,0x03,0xFC,0x01,0x7C,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const char chr_f56_30[] =         // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x07,0x00,0x00,0xFC,0x3F,0x00,0x00,0xFF,0x7F,0x00,0x80,0xFF,0xFF,0x00,0xC0,0x1F,0xF8,0x01,0xC0,0x07,0xF0,0x03,0xE0,0x03,0xE0,0x03,0xE0,0x03,0xC0,0x07,0xF0,0x01,0x80,0x07,0xF0,0x01,0x80,0x0F,0xF0,0x00,0x80,0x0F,0xF0,0x00,0x00,0x0F,0xF8,0x00,0x00,0x0F,0xF8,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0x78,0x00,0x00,0x1F,0x78,0x00,0x00,0x1F,0x78,0x00,0x00,0x1E,0x78,0x00,0x00,0x1E,0x78,0x00,0x00,0x1E,0x78,0x00,0x00,0x1E,0x78,0x00,0x00,0x1F,0x78,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF0,0x00,0x00,0x0F,0xF0,0x00,0x80,0x0F,0xF0,0x01,0x80,0x0F,0xE0,0x01,0x80,0x07,0xE0,0x03,0xC0,0x07,0xE0,0x03,0xE0,0x07,0xC0,0x07,0xE0,0x03,0x80,0x1F,0xF8,0x01,0x80,0xFF,0xFF,0x01,0x00,0xFE,0xFF,0x00,0x00,0xFC,0x3F,0x00,0x00,0xF0,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const char chr_f56_31[] =         // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x0F,0x00,0x00,0xC0,0x0F,0x00,0x00,0xF0,0x0F,0x00,0x00,0xF8,0x0F,0x00,0x00,0xFC,0x0F,0x00,0x00,0xBF,0x0F,0x00,0x80,0x9F,0x0F,0x00,0xC0,0x8F,0x0F,0x00,0xC0,0x87,0x0F,0x00,0xC0,0x81,0x0F,0x00,0x80,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0
};
const char chr_f56_32[] =         // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x07,0x00,0x00,0xFF,0x3F,0x00,0xC0,0xFF,0xFF,0x00,0xE0,0xFF,0xFF,0x01,0xF0,0x0F,0xFC,0x01,0xF0,0x01,0xF0,0x03,0x60,0x00,0xE0,0x07,0x20,0x00,0xC0,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0x80,0x07,0x00,0x00,0x80,0x07,0x00,0x00,0x80,0x07,0x00,0x00,0x80,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0xE0,0x03,0x00,0x00,0xE0,0x03,0x00,0x00,0xF0,0x01,0x00,0x00,0xF8,0x01,0x00,0x00,0xF8,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x3F,0x00,0x00,0x80,0x1F,0x00,0x00,0xC0,0x0F,0x00,0x00,0xE0,0x07,0x00,0x00,0xF0,0x03,0x00,0x00,0xF8,0x01,0x00,0x00,0xFC,0x00,0x00,0x00,0x7E,0x00,0x00,0x00,0x3F,0x00,0x00,0x80,0x1F,0x00,0x00,0xC0,0x0F,0x00,0x00,0xE0,0x07,0x00,0x00,0xF0,0x03,0x00,0x00,0xF8,0x01,0x00,0x00,0xF8,0xFF,0xFF,0x1F,0xF8,0xFF,0xFF,0x1F,0xF8,0xFF,0xFF,0x1F,0xF8,0xFF,0xFF,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const char chr_f56_33[] =         // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x0F,0x00,0x00,0xFF,0x3F,0x00,0xE0,0xFF,0xFF,0x00,0xF0,0xFF,0xFF,0x01,0xF8,0x07,0xF8,0x03,0xF0,0x00,0xE0,0x03,0x30,0x00,0xC0,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0x80,0x07,0x00,0x00,0x80,0x07,0x00,0x00,0x80,0x07,0x00,0x00,0x80,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0xE0,0x07,0x00,0x00,0xE0,0x03,0x00,0x00,0xF8,0x01,0x00,0x00,0xFE,0x00,0x00,0xFE,0x7F,0x00,0x00,0xFE,0x0F,0x00,0x00,0xFE,0x3F,0x00,0x00,0xFE,0xFF,0x01,0x00,0x00,0xFC,0x03,0x00,0x00,0xF0,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0xC0,0x07,0x18,0x00,0xE0,0x07,0xF8,0x01,0xFC,0x03,0xF8,0xFF,0xFF,0x01,0xF8,0xFF,0xFF,0x00,0xF0,0xFF,0x3F,0x00,0x00,0xFF,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const char chr_f56_34[] =         // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x01,0x00,0x00,0xF8,0x01,0x00,0x00,0xFC,0x01,0x00,0x00,0xFC,0x01,0x00,0x00,0xFE,0x01,0x00,0x00,0xEF,0x01,0x00,0x00,0xEF,0x01,0x00,0x80,0xE7,0x01,0x00,0xC0,0xE7,0x01,0x00,0xE0,0xE3,0x01,0x00,0xE0,0xE1,0x01,0x00,0xF0,0xE1,0x01,0x00,0xF8,0xE0,0x01,0x00,0x78,0xE0,0x01,0x00,0x3C,0xE0,0x01,0x00,0x3E,0xE0,0x01,0x00,0x1E,0xE0,0x01,0x00,0x0F,0xE0,0x01,0x80,0x0F,0xE0,0x01,0xC0,0x07,0xE0,0x01,0xC0,0x03,0xE0,0x01,0xE0,0x03,0xE0,0x01,0xF0,0x01,0xE0,0x01,0xF0,0x00,0xE0,0x01,0x78,0x00,0xE0,0x01,0x7C,0x00,0xE0,0x01,0x3C,0x00,0xE0,0x01,0xFE,0xFF,0xFF,0x7F,0xFE,0xFF,0xFF,0x7F,0xFE,0xFF,0xFF,0x7F,0xFE,0xFF,0xFF,0x7F,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const char chr_f56_35[] =         // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xC0,0xFF,0xFF,0x03,0xC0,0xFF,0xFF,0x03,0xC0,0xFF,0xFF,0x03,0xC0,0xFF,0xFF,0x03,0xC0,0x03,0x00,0x00,0xC0,0x03,0x00,0x00,0xC0,0x03,0x00,0x00,0xE0,0x03,0x00,0x00,0xE0,0x03,0x00,0x00,0xE0,0x03,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0x01,0x00,0x00,0xE0,0xFD,0x0F,0x00,0xE0,0xFF,0x7F,0x00,0xE0,0xFF,0xFF,0x00,0xE0,0xFF,0xFF,0x03,0xC0,0x01,0xFC,0x03,0x00,0x00,0xE0,0x07,0x00,0x00,0xC0,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0xC0,0x07,0x30,0x00,0xF0,0x07,0xF0,0x01,0xFC,0x03,0xF0,0xFF,0xFF,0x01,0xF0,0xFF,0xFF,0x00,0xE0,0xFF,0x3F,0x00,0x00,0xFF,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const char chr_f56_36[] =         // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x03,0x00,0xE0,0xFF,0x03,0x00,0xF0,0xFF,0x03,0x00,0xFC,0xFF,0x03,0x00,0xFE,0x01,0x02,0x00,0x7F,0x00,0x00,0x80,0x1F,0x00,0x00,0x80,0x0F,0x00,0x00,0xC0,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0xE0,0x03,0x00,0x00,0xE0,0x03,0x00,0x00,0xE0,0x01,0x00,0x00,0xF0,0x01,0x00,0x00,0xF0,0x01,0x00,0x00,0xF0,0x00,0x00,0x00,0xF0,0xC0,0x1F,0x00,0xF0,0xF8,0xFF,0x00,0xF8,0xFC,0xFF,0x01,0xF8,0xFE,0xFF,0x03,0xF8,0x3F,0xF0,0x07,0xF8,0x07,0xC0,0x0F,0xF8,0x03,0x80,0x0F,0xF8,0x01,0x00,0x1F,0xF8,0x01,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1E,0xF8,0x00,0x00,0x1E,0xF0,0x00,0x00,0x1E,0xF0,0x00,0x00,0x1E,0xF0,0x00,0x00,0x1E,0xF0,0x00,0x00,0x1F,0xF0,0x01,0x00,0x1F,0xE0,0x01,0x00,0x1F,0xE0,0x03,0x80,0x0F,0xC0,0x07,0x80,0x0F,0xC0,0x0F,0xC0,0x07,0x80,0x3F,0xF0,0x07,0x00,0xFF,0xFF,0x03,0x00,0xFE,0xFF,0x01,0x00,0xFC,0x7F,0x00,0x00,0xE0,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const char chr_f56_37[] =         // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0xFF,0xFF,0x1F,0xF8,0xFF,0xFF,0x1F,0xF8,0xFF,0xFF,0x1F,0xF8,0xFF,0xFF,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0xE0,0x03,0x00,0x00,0xE0,0x03,0x00,0x00,0xF0,0x01,0x00,0x00,0xF0,0x01,0x00,0x00,0xF0,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x78,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x3C,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0xC0,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0xC0,0x03,0x00,0x00,0xE0,0x03,0x00,0x00,0xE0,0x03,0x00,0x00,0xF0,0x01,0x00,0x00,0xF0,0x01,0x00,0x00,0xF8,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x7C,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const char chr_f56_38[] =         // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x0F,0x00,0x00,0xFE,0x7F,0x00,0x00,0xFF,0xFF,0x00,0xC0,0xFF,0xFF,0x03,0xC0,0x1F,0xF8,0x03,0xE0,0x07,0xE0,0x07,0xE0,0x03,0xC0,0x07,0xF0,0x01,0x80,0x0F,0xF0,0x01,0x80,0x0F,0xF0,0x01,0x80,0x0F,0xF0,0x01,0x80,0x0F,0xF0,0x01,0x80,0x0F,0xF0,0x01,0x80,0x07,0xE0,0x03,0xC0,0x07,0xE0,0x03,0xC0,0x03,0xC0,0x07,0xE0,0x03,0x80,0x1F,0xF8,0x01,0x00,0x7F,0xFE,0x00,0x00,0xFE,0x3F,0x00,0x00,0xFC,0x1F,0x00,0x00,0xFC,0x1F,0x00,0x00,0xFE,0x7F,0x00,0x80,0x3F,0xFF,0x01,0xC0,0x0F,0xF8,0x03,0xE0,0x07,0xF0,0x07,0xF0,0x03,0xC0,0x0F,0xF0,0x01,0x80,0x0F,0xF8,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0x78,0x00,0x00,0x1F,0x78,0x00,0x00,0x1E,0x78,0x00,0x00,0x1E,0x78,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF0,0x01,0x80,0x0F,0xF0,0x03,0xC0,0x0F,0xE0,0x0F,0xF0,0x07,0xC0,0xFF,0xFF,0x03,0x80,0xFF,0xFF,0x01,0x00,0xFF,0x7F,0x00,0x00,0xF8,0x1F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const char chr_f56_39[] =         // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF0,0x07,0x00,0x00,0xFE,0x3F,0x00,0x00,0xFF,0x7F,0x00,0x80,0xFF,0xFF,0x00,0xC0,0x1F,0xFC,0x01,0xE0,0x07,0xF0,0x03,0xF0,0x03,0xE0,0x03,0xF0,0x01,0xC0,0x07,0xF0,0x00,0x80,0x07,0xF8,0x00,0x80,0x0F,0xF8,0x00,0x00,0x0F,0xF8,0x00,0x00,0x0F,0x78,0x00,0x00,0x1F,0x78,0x00,0x00,0x1F,0x78,0x00,0x00,0x1F,0x78,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF8,0x00,0x00,0x1F,0xF0,0x00,0x80,0x1F,0xF0,0x01,0xC0,0x1F,0xF0,0x03,0xE0,0x1F,0xE0,0x0F,0xF8,0x1F,0xC0,0xFF,0x7F,0x1F,0x80,0xFF,0x3F,0x1F,0x00,0xFF,0x1F,0x1F,0x00,0xF8,0x03,0x0F,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x0F,0x00,0x00,0x80,0x07,0x00,0x00,0xC0,0x07,0x00,0x00,0xC0,0x03,0x00,0x00,0xE0,0x03,0x00,0x00,0xF0,0x01,0x00,0x00,0xF8,0x01,0x00,0x00,0xFC,0x00,0x40,0x80,0x7F,0x00,0xC0,0xFF,0x3F,0x00,0xC0,0xFF,0x1F,0x00,0xC0,0xFF,0x07,0x00,0xC0,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
const char chr_f56_3A[] =          // 4 bytes per row
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x80,0x0F,0xE0,0x0F,0xF0,0x07,0xF8,0x03,0xF8,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x07,0xC0,0x07,0xF0,0x07,0xF8,0x03,0xFC,0x01,0x7C,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

const char* const chrtbl_f56[96] =        // character pointer table
{
    chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20,
    chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_2E, chr_f56_20,
    chr_f56_30, chr_f56_31, chr_f56_32, chr_f56_33, chr_f56_34, chr_f56_35, chr_f56_36, chr_f56_37,
    chr_f56_38, chr_f56_39, chr_f56_3A, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20,
    chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20,
    chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20,
    chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20,
    chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20,
    chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20,
    chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20,
    chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20,
    chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20, chr_f56_20
};
