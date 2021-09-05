
#include "config.h"
#include <Arduino.h>

#include "debug.h"
#include "cmenue.h"
#include "cMeanCalc.h"
#include "InternalTemperature.h"
#include "cLog.h"

cMeanCalc<int16_t,10> digits;

void initPins()
{
  pinMode(PIN_AMP_0, OUTPUT);
  pinMode(PIN_AMP_1, OUTPUT);
  pinMode(PIN_AMP_2, OUTPUT);
  pinMode(PIN_AMP_3, OUTPUT);

  pinMode(PIN_ID_12V, INPUT_PULLUP);

  pinMode(PIN_TFT_LED, OUTPUT);
  pinMode(PIN_POWER_OFF, OUTPUT);
  digitalWrite(PIN_POWER_OFF, 1);
}

void checkSupplyVoltage()
{
  float volt =readSupplyVoltage();
  bool ok = false;
  if(digitalRead(PIN_ID_12V) == 1)
    ok = volt >= SUPPLY_4V_MIN;
  else 
    ok = volt >= SUPPLY_12V_MIN;
  if(!ok)
  {
    digitalWrite(PIN_POWER_OFF, 0);
    cLog::logf("power down voltage too low : %f \n ", volt);
    delay(1000);
  }    
}

float readSupplyVoltage()
{
  float volt = 0;
  int digs = digits.get(analogRead(PIN_SUPPLY_VOLT));
  devStatus.digits.set((float)digs);

  if(digitalRead(PIN_ID_12V) == 1)
    volt = (float)digs * devPars.voltFactor.get() + U_DIODE;
  else
    volt = (float)digs * devPars.voltFactor.get();

  DPRINTF2("digits: %i  voltage: %f  factor: %f\n", digs, volt,devPars.voltFactor.get());
  return volt;
}

float calcVoltageFactor(float volt)
{
  float fact = 1.0;
  int digits = analogRead(PIN_SUPPLY_VOLT);
  if(digits > 0)
  {
    if(digitalRead(PIN_ID_12V) == 1)
      fact = (devStatus.voltage.get() - U_DIODE) / digits;
    else
      fact = (devStatus.voltage.get()) / digits;
  }
  DPRINTF2("digits: %i, voltage: %f, factor: %f\n", digits, devStatus.voltage.get(), fact);
  return fact;
}

float readTemperature()
{
  if(digitalRead(PIN_ID_12V) == 1)
    return InternalTemperature.readTemperatureC() - TEMP_OFFS_PORTABLE;
  else
    return InternalTemperature.readTemperatureC() - TEMP_OFFS_STATIONARY;
}