#include <iostream>
#include <cstring>
#include <conio.h>
#include "RawSerial.h"
#include "DigitalOut.h"
#include "Ticker.h"
#include "Timeout.h"
#include "Callback.h"

#ifndef MBED_H
#define MBED_H

typedef int PinName;
typedef int BusOut;

enum Pins {
    p5 = 5,
    p6,
    p7,
    p8,
    p9,
    p10,
    p11,
    p12,
    p13,
    p14,
    p15,
    p16,
    p17,
    p18,
    p19,
    p20,
    p21,
    p22,
    p23,
    p24,
    p25,
    p26,
    p27,
    p28,
    p29,
    p30
};

void wait(int value);
void wait_us(int t);
void wait_ms(int t);

#endif
