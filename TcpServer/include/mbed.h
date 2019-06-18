#include <new>
#include <cstring>
#include <cstddef>
#include <cstdint>
#include "Timer.h"
#include "BusOut.h"
#include "Serial.h"
#include "RawSerial.h"
#include "DigitalIn.h"
#include "DigitalOut.h"
#include "Ticker.h"
#include "Timeout.h"
#include "Callback.h"
#include "EventQueue.h"

#define SIMULATED

#ifndef MBED_H
#define MBED_H

typedef int PinName;

enum PullModeT {
    PullUp,
    PullDown
};

enum Pins {
    NC = 0,
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
    p30,
    LED1,
    LED2,
    LED3,
    LED4,
    USBTX,
    USBRX
};

void wait(int value);
void wait_us(int t);
void wait_ms(int t);

#endif
