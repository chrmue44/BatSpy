#ifndef _CONFIG_H
#define _CONFIG_H

// debug level: 1(lowest) .. 4(highest) or undefine for no messages
//#define DEBUG_LEVEL   2
// Teensy version
//#define TEENSY36

//pre amplifier type
//#define AMP_REV1               // 1st prototype
#define AMP_REV2                 // 2nd version with more amplification

//#define AUDIO_IN_SPI
#define AUDIO_OUT_TEENSY

// *****   pin definitions *****
//control pre amplifier
#define PIN_AMP_0        15      // enable feedback resistor
#define PIN_AMP_1        29      // enable feedback resistor
#define PIN_AMP_2        16      // enable pre amp linear
#define PIN_AMP_3        30      // enable pre amp with high pass

//encoder
#define PIN_ROT_LEFT_A   36      // encoder input A
#define PIN_ROT_LEFT_B   17      // encoder input B
#define PIN_ROT_LEFT_S   37      // encoder push button
#define PIN_ROT_RIGHT_A  55
#define PIN_ROT_RIGHT_B  56
#define PIN_ROT_RIGHT_S  57

#ifdef TEENSY36
// control TFT
#define PIN_TFT_DC        6      // TFT D/C-Signal
#define PIN_TFT_CS       14      // TFT CS
#define PIN_TFT_RST      24      // TFT Reset
#define PIN_TFT_MOSI      7      // TFT MOSI
#define PIN_TFT_SCLK     27      // TFT SCLK
#define PIN_TFT_MISO     12      // TFT MISO
#define PIN_TFT_LED     A12      // TFT backlight control

// control SGTL5000
#define PIN_DIN          22
#define PIN_DOUT         13
#define PIN_LRCLK        23
#define PIN_SCL          19
#define PIN_SDA          18
#define PIN_BCLK          9
#define PIN_MCLK         11
#endif //#ifdef TEENSY36

#ifdef ARDUINO_TEENSY41
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
#endif //#ifdef ARDUINO_TEENSY41


#endif  //#ifndef _CONFIG_H

