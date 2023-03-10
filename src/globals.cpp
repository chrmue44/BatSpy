/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "globals.h"

cLog sysLog("sys");
cLog gpsLog("gps");
///cGps gps;
ILI9341_t3 tft = ILI9341_t3(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST, 
                            PIN_TFT_MOSI, PIN_TFT_SCLK, PIN_TFT_MISO);
///cAudio audio;  // audio control
///cRtc rtc;
cMenue menue(320, 240, &tft);

Metro tick300ms(300);
Metro tick1s(1000);
Metro tick15Min(1000 * 60 * 15);
///cWheels wheels(PIN_ROT_LEFT_A, PIN_ROT_LEFT_B, PIN_ROT_LEFT_S,
///               PIN_ROT_RIGHT_A, PIN_ROT_RIGHT_B, PIN_ROT_RIGHT_S);
/**
cTerminal terminal;
**/
stStatus devStatus;          ///< status of the device
stParams devPars;            ///< parameters of the device
