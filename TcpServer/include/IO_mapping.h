#ifndef GLOBAL_DEFINITIONS_H
#define GLOBAL_DEFINITIONS_H

#include "mbed.h"

const PinName SPI_MOSI_PIN 		= p5;
const PinName SPI_MISO_PIN 		= p6;
const PinName SPI_SCK_PIN 		= p7;

const PinName MPC4161_CS_PIN	= p8;
const PinName POWER_SW_PIN		= p9;

const PinName TFT_A0_PIN		= p10;
const PinName TFT_CS_PIN		= p11;
const PinName TFT_LED_PIN		= p12;

const PinName SERIAL_TX			= p13;
const PinName SERIAL_RX			= p14;

const PinName TFT_RESET_PIN		= p15;

const PinName IR_RX_PIN			= p17;

const PinName INPUT_SW_PIN		= p18;
const PinName CLK_PIN			= p19;
const PinName DT_PIN			= p20;

const PinName POWER_LED_PIN		= p21;

const PinName ESP8266_RST_PIN	= p22;

const PinName INP1_EN_PIN		= p23;
const PinName INP2_EN_PIN		= p24;
const PinName INP3_EN_PIN		= p25;
const PinName INP4_EN_PIN		= p26;

// AK4113 I2C
const PinName I2C_SCL_PIN		= p27;
const PinName I2C_SDA_PIN		= p28;
const PinName AK4113_PDN_PIN	= p29;
const PinName AK4113_SDTO_PIN	= p30;

#endif

