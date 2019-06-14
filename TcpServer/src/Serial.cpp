#include <iostream>
#include <cstdarg>
#include "conio.h"
#include "Serial.h"

Serial::Serial(PinName TxPin, PinName RxPin, int baudrate)
{

}

Serial::~Serial()
{
    std::cin.get();
}

void Serial::printf(const char* format, ...)
{
    va_list args;
    va_start (args, format);
    vprintf (format, args);
    va_end (args);
}

void Serial::putc(char c)
{
    //std::cout << c;
}

void Serial::readFromFile(const char* filename, char* buff, int len)
{
    memset(buff, '\0', len);

	FILE* fp = fopen(filename, "r");
	if (fp != NULL) {
	    char c;
	    char* wp = buff;
        while ((c = fgetc(fp)) != EOF) {
            *wp++ = c;
        }
        fclose(fp);
	}
}

int Serial::waitKeyPress()
{
    return _kbhit();
}
