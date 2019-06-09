#include <chrono>
#include <thread>
#include "mbed.h"

void wait(int t)
{
    std::this_thread::sleep_for(std::chrono::seconds(t));
}

void wait_us(int t) {
    static int wait_counter = 0;
    if ((wait_counter % 10) == 0) {
        wait_ms(10);
    }
    wait_counter++;
}

void wait_ms(int t) {
    std::this_thread::sleep_for(std::chrono::milliseconds(t));
}
