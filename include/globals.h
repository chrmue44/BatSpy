/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/
#ifndef GLOBALS_H
#define GLOBALS_H

#ifdef ARDUINO_TEENSY40
#include <TCA9534.h>
extern TCA9534 ioex;
#endif

#include "cLog.h"
#include "cgps.h"
#include "Adafruit_GFX.h"
#include "config.h"
#include "cAudio.h"
#include "cmenue.h"
#include <Metro.h>
#include "cWheels.h"
#include "cTerminal.h"
#include "cnotes.h"
#include "cStatus.h"
#include "cEeprom.h"
#include "SensirionI2cSht4x.h"
#include "cCmdExecuter.h"


extern cLog sysLog;
extern cLog gpsLog;
//extern cLog trigLog;
extern cGps gps;
extern Adafruit_GFX* pDisplay;
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
extern SensirionI2cSht4x sht;
extern cStatus statusDisplay;
extern char serialNumber[EEP_SERIAL_SIZE];

extern cCmdExecuter command;
#endif //#ifndef GLOBALS_H
