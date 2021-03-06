/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * All hardware dependent configuration  
 * ***********************************************************/
#ifndef _CONFIG_H
#define _CONFIG_H
/*
                  +---------+    +---------+
                --| GND     +----+     VIN |--
                --| D0                 GND |--
                --| D1   Teensy 4.1    3V3 |--
                --| D2              A9 D23 |--
                --| D3              A8 D22 |--
                --| D4              A7 D21 |-- BCLK_ADC
                --| D5              A6 D20 |-- LRCLK_ADC
         TFT_DC --| D6              A5 D19 |--
                --| D7              A4 D18 |--
                --| D8              A3 D17 |-- ROT_LEFT_B
                --| D9              A2 D16 |-- AMP2
         TX_ADC --| D10             A1 D15 |-- AMP0
       TFT_MOSI --| D11             A0 D14 |-- TFT_CS
       TFT_MISO --| D12                D13 |--
       TFT_SCLK --| 3V3                GND |--
        TFT_RST --| D24 A10        A17 D41 |--
                --| D25 A11        A16 D40 |-- SUPPLY_VOLT
                --| D26 A12        A15 D39 |--
                --| D27 A13        A14 D38 |-- POWER_OFF
                --| D28                D37 |-- ROT_LEFT_S
           AMP1 --| D29                D36 |-- ROT_LEFT_A
           AMP3 --| D30                D35 |-- TX_GPS
        TFT_LED --| D31                D34 |-- TX_GPS
                --| D32                D33 |--
                  +------------------------+
*/
// memeory control
#define MEMP FLASHMEM     //memory region for user panel functions
#define MEMF FLASHMEM     //memory region for user file handling functions

// voltage loss D1
#define U_DIODE           0.29
#define SUPPLY_12V_MIN   11.5     //min. required supply voltage lead 
#define SUPPLY_4V_MIN     3.0     //min. required supply voltage LiIon 

#define TEMP_OFFS_PORTABLE   34.0   // temp offset measured CPU temp to outside temp
#define TEMP_OFFS_STATIONARY 25.0   // temp offset measured CPU temp to outside temp

//#define AUDIO_IN_SPI
#define AUDIO_OUT_TEENSY

// *****   pin definitions *****

#define PIN_POWER_OFF    38      // power off if voltage too low

// identification
#define PIN_ID_12V        4      // low level == 12V supply

//control pre amplifier
#define PIN_AMP_0        15      // enable feedback resistor
#define PIN_AMP_1        29      // enable feedback resistor
#define PIN_AMP_2        16      // enable feedback resistor
#define PIN_AMP_3        30      // enable pre amp with high pass

//encoder
#define PIN_ROT_LEFT_A   36      // encoder input A
#define PIN_ROT_LEFT_B   17      // encoder input B
#define PIN_ROT_LEFT_S   37      // encoder push button
#define PIN_ROT_RIGHT_A  55
#define PIN_ROT_RIGHT_B  56
#define PIN_ROT_RIGHT_S  57

#define PIN_SUPPLY_VOLT  A16

// audio out
#define PIN_MQS          10      // output for mono MQS signal

// control TFT
#define PIN_TFT_DC        6      // TFT D/C-Signal
#define PIN_TFT_CS       14      // TFT CS
#define PIN_TFT_RST      24      // TFT Reset
#define PIN_TFT_MOSI     11      // TFT MOSI
#define PIN_TFT_SCLK     13      // TFT SCLK
#define PIN_TFT_MISO     12      // TFT MISO
#define PIN_TFT_LED      31      // TFT backlight control

// control SGTL5000
#define PIN_DIN           7
#define PIN_DOUT          8
#define PIN_LRCLK        20
#define PIN_SCL          19
#define PIN_SDA          18
#define PIN_BCLK         21
#define PIN_MCLK         23

#define SERIAL_GPS       Serial8

float readSupplyVoltage();
void initPins();
float calcVoltageFactor(float volt);
void checkSupplyVoltage();
float readTemperature();

#endif  //#ifndef _CONFIG_H
