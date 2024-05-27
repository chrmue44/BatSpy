/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "globals.h"

cWheels wheels(PIN_ROT_LEFT_A, PIN_ROT_LEFT_B, PIN_ROT_LEFT_S);
cLog sysLog("sys");
cLog gpsLog("gps");
//cLog trigLog("trig");
cGps gps;
Adafruit_GFX* pDisplay = nullptr;
cAudio audio;  // audio control
cRtc rtc;
//cMenue menue(320, 240, &tft);
cMenue menue(320, 240, pDisplay);

Metro tick300ms(300);
Metro tick1s(1000);
Metro tick15Min(1000 * 60 * 15);
cTerminal terminal;
cnotes notes1;
cnotes notes2;
cStatus statusDisplay;

stStatus devStatus;          ///< status of the device
stParams devPars;            ///< parameters of the device
SensirionI2cSht4x sht;
char serialNumber[EEP_SERIAL_SIZE];

#ifdef ARDUINO_TEENSY40
TCA9534 ioex;
#endif
