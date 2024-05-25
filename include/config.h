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
        AMP3_REVB --| D0                 GND |--
        AMP1_REVB --| D1   Teensy 4.1    3V3 |--
                  --| D2              A9 D23 |--
                  --| D3              A8 D22 |--
           ID_12V --| D4              A7 D21 |-- BCLK_ADC
             REV1 --| D5              A6 D20 |-- LRCLK_ADC  
           TFT_DC --| D6              A5 D19 |--
 (Serial2) RX_ESP --| D7              A4 D18 |--
 (Serial2) TX_ESP --| D8              A3 D17 |-- ROT_LEFT_B
             REV2 --| D9              A2 D16 |-- AMP2
           TX_ADC --| D10             A1 D15 |-- AMP0
         TFT_MOSI --| D11             A0 D14 |-- TFT_CS
         TFT_MISO --| D12                D13 |-- TFT_SCLK
                  --| 3V3                GND |--
          TFT_RST --| D24 A10        A17 D41 |-- U_REF_ADC
                  --| D25 A11        A16 D40 |-- SUPPLY_VOLT
                  --| D26 A12        A15 D39 |--
                  --| D27 A13        A14 D38 |-- POWER_OFF
             REV3 --| D28                D37 |-- ROT_LEFT_S
        AMP1_REVA --| D29                D36 |-- ROT_LEFT_A
        AMP3_REVA --| D30                D35 |-- TX_GPS (Serial8)
          TFT_LED --| D31                D34 |-- RX_GPS (Serial8)
                  --| D32                D33 |--
                    +------------------------+

                    +---------+    +---------+
                  --| GND     +----+     VIN |--
              CS1 --| D0                 GND |--
         TFT_MISO --| D1   Teensy 4.0    3V3 |--
           ROT_SW --| D2              A9 D23 |-- CS_SDCARD
           SPARE1 --| D3              A8 D22 |-- SUPPLY_VOLT
             REV0 --| D4              A7 D21 |-- BCLK_ADC
             REV1 --| D5              A6 D20 |-- LRCLK_ADC  
           TFT_DC --| D6              A5 D19 |-- I2C_SCL
              DIN --| D7              A4 D18 |-- I2C_SDA
         TX_CODEC --| D8              A3 D17 |-- TX_GPS (Serial 4)
             REV2 --| D9              A2 D16 |-- RX_GPS (Serial 4)
           SPARE2 --| D10             A1 D15 |-- ROT_B
        (SD) MOSI --| D11             A0 D14 |-- ROT_A
        (SD) MISO --| D12                D13 |-- SCLK (SD)
                    +------------------------+

*/

#include <cstdint>
#include "Arduino.h"
#include "types.h"
#include "Adafruit_GFX.h"

// memeory control
#define MEMP FLASHMEM     //memory region for user panel functions
#define MEMF FLASHMEM     //memory region for user file handling functions

#define TEMP_OFFS_PORTABLE   34.0f   // temp offset measured CPU temp to outside temp
#define TEMP_OFFS_STATIONARY 25.0f   // temp offset measured CPU temp to outside temp

#define CPU_FREQ_HIGH      200000000  // CPU frequency for full speed operation
#define CPU_FREQ_LOW        24000000  // CPU frequency for reduced speed operation

//#define AUDIO_IN_SPI
#define AUDIO_OUT_TEENSY

#ifdef ARDUINO_TEENSY40
// ****************************************************************

#define I2C_ADDR_PORT_EXT 0x20
#define I2C_ADDR_OLED     0x3C
#define I2C_ADDR_HUMID_A  0x44
#define I2C_ADDR_HUMID_B  0x45


// *****   pin definitions *****
#define SPIN_POWER_OFF   0x8010      // power off if voltage too low
#define SPIN_LED_DISP    0x8002      // LED Display (D5)
#define SPIN_LED_2       0x8001      // LED 2       (D4)
#define SPIN_AMP0        0x8004      // AMP0
#define SPIN_TFT_RES     0x8008      // Reset TFT-Display
#define SPIN_PWR_ANA     0x8020      // Poser supply analog stage
#define SPIN_PWR_GPS     0x8040      // power supply GPS
#define SPIN_SPARE3      0x8080      // spare pin

#define PIN_REV0         4           // 
#define PIN_REV1         5           // revision bit 0
#define PIN_REV2         9           // revision bit 1

#define PIN_ROT_LEFT_A   14          // encoder input A
#define PIN_ROT_LEFT_B   15          // encoder input B
#define PIN_ROT_LEFT_S   2           // encoder push button

#define PIN_SUPPLY_VOLT  A8    

// audio out
#define PIN_MQS          10      // output for mono MQS signal

// control TFT
#define PIN_TFT_DC_REVA   6      // TFT D/C-Signal
#define PIN_TFT_CS       0       // TFT CS
#define PIN_TFT_MOSI     26      // TFT MOSI
#define PIN_TFT_SCLK     27      // TFT SCLK
#define PIN_TFT_MISO     3      // TFT MISO

#define SERIAL_GPS       Serial4

inline bool hasAmpRevB() { return  false; }
inline bool isRevisionB() { return false; }
void portExpSetBit(uint8_t port, uint8_t state);
#endif //ARDUINO_TEENSY40

#ifdef ARDUINO_TEENSY41
// ****************************************************************
extern uint8_t pinAmp0;
extern uint8_t pinAmp1;
extern uint8_t pinAmp2;
extern uint8_t pinAmp3;

// *****   pin definitions *****

#define PIN_POWER_OFF    38      // power off if voltage too low

// identification
#define PIN_ID_12V        4      // low level == 12V supply
#define PIN_REV1          5      // revision bit 0
#define PIN_REV2          9      // revision bit 1
#define PIN_REV3         28      // revision bit 2

//control pre amplifier
#define PIN_AMP_0_REVA   15      // enable feedback resistor
#define PIN_AMP_1_REVA   29      // enable feedback resistor
#define PIN_AMP_2_REVA   16      // enable feedback resistor
#define PIN_AMP_3_REVA   30      // enable pre amp with high pass

#define PIN_AMP_0_REVB   15      // enable feedback resistor
#define PIN_AMP_1_REVB    1      // enable feedback resistor
#define PIN_AMP_2_REVB   16      // enable feedback resistor
#define PIN_AMP_3_REVB    0      // enable feedback resistor

//encoder
#define PIN_ROT_LEFT_A   36      // encoder input A
#define PIN_ROT_LEFT_B   17      // encoder input B
#define PIN_ROT_LEFT_S   37      // encoder push button
#define PIN_ROT_RIGHT_A  55
#define PIN_ROT_RIGHT_B  56
#define PIN_ROT_RIGHT_S  57

#define PIN_SUPPLY_VOLT  A16
#define PIN_U_REF_ADC    A17
#define PIN_SIG_LAMP     23      // external output

// audio out
#define PIN_MQS          10      // output for mono MQS signal

// control TFT
#define PIN_TFT_DC_REVA   6      // TFT D/C-Signal
#define PIN_TFT_DC_REVB   8      // TFT D/C-Signal
#define PIN_TFT_CS       14      // TFT CS
#define PIN_TFT_RST      24      // TFT Reset
#define PIN_TFT_MOSI     11      // TFT MOSI
#define PIN_TFT_SCLK     13      // TFT SCLK
#define PIN_TFT_MISO     12      // TFT MISO
#define PIN_TFT_LED_REVA 31      // TFT backlight control
#define PIN_TFT_LED_REVB 39      // TFT backlight control

// control SGTL5000
#define PIN_DIN           7
#define PIN_DOUT          8
#define PIN_LRCLK        20
#define PIN_SCL          19
#define PIN_SDA          18
#define PIN_BCLK         21
#define PIN_MCLK         23

#define SERIAL_GPS       Serial8

inline bool isRevisionB() { return (digitalRead(PIN_REV1) == 0); }
inline bool isRevisionA() { return (digitalRead(PIN_REV1) == 1); }
inline bool hasAmpRevB() { return  isRevisionB(); }


#define portExpSetBit(x, y)   digitalWrite(x, y)

#endif // defined (ARDUINO_TEENSY41)
int hasDisplay();
void testDisplay();
float readSupplyVoltage();
void initPins();
float calcVoltageFactor(float volt);
void checkSupplyVoltage();
float readTemperature(float& humidity);
void setDispLight(uint8_t bright);
void digWrite(int pin, uint8_t stat);
void resetTft();
void powerOff();
void setDisplayBrightness(uint8_t brightness);
void setIoDebugMode(bool mode);
void portExpSetBit(uint8_t port, uint8_t state);
void initDisplay(int orientation, uint8_t brightness, bool showSplash = true);
void showSplashScreen(Adafruit_GFX& tft, bool waitBtn);
void setAnalogPower(bool on);
void setHwOperationMode(enCpuMode mode);
void initTempSensor();

#define PATH_NOTES1  "/info/notes1.txt"
#define PATH_NOTES2  "/info/notes2.txt"

#endif  //#ifndef _CONFIG_H
