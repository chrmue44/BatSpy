#include "config.h"
#include <Arduino.h>
#include "debug.h"
#include "cmenue.h"

void initPins()
{
  pinMode(PIN_AMP_0, OUTPUT);
  pinMode(PIN_AMP_1, OUTPUT);
  pinMode(PIN_AMP_2, OUTPUT);
  pinMode(PIN_AMP_3, OUTPUT);

  pinMode(PIN_ID_12V, INPUT_PULLUP);

  pinMode(PIN_TFT_LED, OUTPUT);
}

float readSupplyVoltage()
{
  float volt = 0;
  int digits = analogRead(PIN_SUPPLY_VOLT);
  devStatus.digits.set((float)digits);

  if(digitalRead(PIN_ID_12V) == 1)
    volt = (float)digits * devPars.voltFactor.get() + U_DIODE;
  else
    volt = (float)digits * devPars.voltFactor.get();

  DPRINTF2("digits: %i  voltage: %f  factor: %f\n", digits, volt,devPars. voltFactor.get());
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
