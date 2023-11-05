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
cMeanCalc<int16_t,10> uref;
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


  if(isRevisionB())
    pinMode(PIN_TFT_LED_REVB, OUTPUT);
  else
    pinMode(PIN_TFT_LED_REVA, OUTPUT);
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
  ok = volt >= devPars.ShutoffVoltage.get();
  if(!ok)
  {
    sysLog.logf("power down voltage too low : %f \n ", volt);
    sysLog.close();
    setDispLight(1);
    menue.showMsg(enMsg::INFO, nullptr, Txt::get(2100));
    delay(500);
    audio.closeProject();
    delay(500);
    cSdCard::inst().unmount();
    delay(500);
    digitalWrite(PIN_POWER_OFF, 0);
  }    
}

float readSupplyVoltage()
{
  float volt = 0;
  int digs = digits.get(analogRead(PIN_SUPPLY_VOLT));
  devStatus.digits.set((float)digs);

  //if(is12V())
    volt = (float)digs * devPars.voltFactor.get();
  /* else
  {
    int digUref = uref.get(analogRead(PIN_U_REF_ADC));
    volt = (float)digs / (float) digUref * U_REF_ADC  * 2 + U_DIODE;
  }*/
  DPRINTF2("digits: %i  voltage: %f  factor: %f\n", digs, volt,devPars.voltFactor.get());
  return volt;
}

float calcVoltageFactor(float volt)
{
  float fact = 1.0;
  int digits = analogRead(PIN_SUPPLY_VOLT);
  if(digits > 0)
  {
      fact = (devStatus.setVoltage.get()) / digits;
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

void setDispLight(uint8_t bright)
{
  uint8_t pinLed;
  if(isRevisionB())
    pinLed = PIN_TFT_LED_REVB;
  else
    pinLed = PIN_TFT_LED_REVA;
  if(bright > 128)
    digitalWrite(pinLed, 1);
  else
    digitalWrite(pinLed, 0);  
}
