/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/
#ifndef GLOBALS_H
#define GLOBALS_H

#include "cLog.h"
#include "cgps.h"
#include "ILI9341_t3.h"
#include "config.h"
#include "cAudio.h"
#include "cmenue.h"
#include <Metro.h>
#include "cWheels.h"
#include "cTerminal.h"
#include "cnotes.h"
#include "cStatus.h"
#include "arduino-sht.h"

#ifdef ARDUINO_TEENSY40
#include <TCA9534.h>
extern TCA9534 ioex;
#endif

extern cLog sysLog;
extern cLog gpsLog;
extern cGps gps;
extern ILI9341_t3 tft;
extern cAudio audio;  // audio control
extern cRtc rtc;
extern cMenue menue;

extern Metro tick300ms;
extern Metro tick1s;
extern Metro tick15Min;

extern cWheels wheels;
extern cTerminal terminal;
extern stStatus devStatus;          ///< status of the device
extern stParams devPars;            ///< parameters of the device
extern cnotes notes1;
extern cnotes notes2;
extern SHTSensor sht;
extern cStatus statusDisplay;
#endif //#ifndef GLOBALS_H
