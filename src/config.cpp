/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "config.h"
#include <Arduino.h>

#include "debug.h"
#include "cmenue.h"
#include "cMeanCalc.h"
#include "InternalTemperature.h"
#include "globals.h"

cMeanCalc<int16_t,10> digits;
uint8_t pinAmp0;
uint8_t pinAmp1;
uint8_t pinAmp2;
uint8_t pinAmp3;

void initPins()
{
  pinMode(PIN_ID_12V, INPUT_PULLUP);
  pinMode(PIN_REV1, INPUT_PULLUP);
  pinMode(PIN_REV2, INPUT_PULLUP);
  pinMode(PIN_REV3, INPUT_PULLUP);
  if (isRevisionA())
  {
    pinAmp0 = PIN_AMP_0_REVA;
    pinAmp1 = PIN_AMP_1_REVA;
    pinAmp2 = PIN_AMP_2_REVA;
    pinAmp3 = PIN_AMP_3_REVA;
  }
  else
  { 
    pinAmp0 = PIN_AMP_0_REVB;
    pinAmp1 = PIN_AMP_1_REVB;
    pinAmp2 = PIN_AMP_2_REVB;
    pinAmp3 = PIN_AMP_3_REVB;
  }

  pinMode(pinAmp0, OUTPUT);
  pinMode(pinAmp1, OUTPUT);
  pinMode(pinAmp2, OUTPUT);
  pinMode(pinAmp3, OUTPUT);



  pinMode(PIN_TFT_LED, OUTPUT);
  pinMode(PIN_POWER_OFF, OUTPUT);
  digitalWrite(PIN_POWER_OFF, 1);
  wheels.init();
  char buf[30];
  if (isRevisionA())
    strncpy(buf, "RevA, ", sizeof(buf));
  else
    strncpy(buf, "RevB, ", sizeof(buf));
  if (is12V())
    strcat(buf, "12V");
  else
    strcat(buf, "5V");
  devStatus.hwVersion.set(buf);
}

void checkSupplyVoltage()
{
  float volt =readSupplyVoltage();
  bool ok = false;
  if(is12V())
    ok = volt >= SUPPLY_12V_MIN;
  else 
    ok = volt >= SUPPLY_4V_MIN;
  if(!ok)
  {
    digitalWrite(PIN_POWER_OFF, 0);
    sysLog.logf("power down voltage too low : %f \n ", volt);
    delay(1000);
    audio.closeProject();
    cSdCard::inst().unmount();
  }    
}

float readSupplyVoltage()
{
  float volt = 0;
  int digs = digits.get(analogRead(PIN_SUPPLY_VOLT));
  devStatus.digits.set((float)digs);

  if(is12V())
    volt = (float)digs * devPars.voltFactor.get();
  else
    volt = (float)digs * devPars.voltFactor.get() + U_DIODE;

  DPRINTF2("digits: %i  voltage: %f  factor: %f\n", digs, volt,devPars.voltFactor.get());
  return volt;
}

float calcVoltageFactor(float volt)
{
  float fact = 1.0;
  int digits = analogRead(PIN_SUPPLY_VOLT);
  if(digits > 0)
  {
    if(is12V())
      fact = (devStatus.voltage.get()) / digits;
    else
      fact = (devStatus.voltage.get() - U_DIODE) / digits;
  }
  DPRINTF2("digits: %i, voltage: %f, factor: %f\n", digits, devStatus.voltage.get(), fact);
  return fact;
}

float readTemperature()
{
  if(is12V())
    return InternalTemperature.readTemperatureC() - TEMP_OFFS_STATIONARY;
  else
    return InternalTemperature.readTemperatureC() - TEMP_OFFS_PORTABLE;
}