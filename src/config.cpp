/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "config.h"
#include <Arduino.h>
#include <Wire.h>

#include "debug.h"
#include "cmenue.h"
#include "cMeanCalc.h"
#include "InternalTemperature.h"
#include "globals.h"

cMeanCalc<int16_t,10> digits;
//cMeanCalc<int16_t,10> uref;

uint8_t ioexOut = 0;

#ifdef ARDUINO_TEENSY41
uint8_t pinAmp0;
uint8_t pinAmp1;
uint8_t pinAmp2;
uint8_t pinAmp3;
#endif

void initPins()
{
  pinMode(PIN_REV1, INPUT_PULLUP);
  pinMode(PIN_REV2, INPUT_PULLUP);
#ifdef ARDUINO_TEENSY40
  pinMode(PIN_REV0, INPUT_PULLUP);
  pinMode(PIN_ROT_LEFT_S, INPUT_PULLUP);
  pinMode(SPIN_LED_DISP, OUTPUT); //@@@
  pinMode(SPIN_LED_2, OUTPUT); //@@@

  Wire.begin();
  ioex.attach(Wire);
  ioex.setDeviceAddress(I2C_ADDR_PORT_EXT);
  ioex.config(TCA9534::Config::OUT); // set all port to output
  ioex.polarity(TCA9534::Polarity::ORIGINAL); // set all port polarity to original
  ioex.output(ioexOut);
  digWrite(SPIN_POWER_OFF, 1);
#endif

#ifdef ARDUINO_TEENSY41
  pinMode(PIN_ID_12V, INPUT_PULLUP);
  pinMode(PIN_REV3, INPUT_PULLUP);
  pinMode(PIN_TFT_RES, OUTPUT);
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
#endif

  wheels.init();
  char buf[30];
#ifdef ARDUINO_TEENSY41
  if (isRevisionA())
    strncpy(buf, "RevA, ", sizeof(buf));
  else
    strncpy(buf, "RevB, ", sizeof(buf));
  if (is12V())
    strcat(buf, "12V");
  else
    strcat(buf, "5V");
#endif
#ifdef ARDUINO_TEENSY40
strncpy(buf, "RevC, ", sizeof(buf));
#endif
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
    if(hasDisplay())
    {
      setDispLight(1);
      menue.showMsg(enMsg::INFO, nullptr, Txt::get(2100));
      delay(500);
    }
    audio.closeProject();
    delay(500);
    cSdCard::inst().unmount();
    delay(500);

    #ifdef ARDUINO_TEENSY41
    digitalWrite(PIN_POWER_OFF, 0);
    #endif
    #ifdef ARDUINO_TEENSY40
    digWrite(SPIN_POWER_OFF, 0);
    #endif
  }    
}

float readSupplyVoltage()
{
  float volt = 0;
  int digs = digits.get(analogRead(PIN_SUPPLY_VOLT));
  devStatus.digits.set((float)digs);

  volt = (float)digs * devPars.voltFactor.get();

  DPRINTF2("digits: %i  voltage: %f  factor: %f\n", digs, volt,devPars.voltFactor.get());
  return volt;
}

void resetTft()
{
#ifdef ARDUINO_TEENSY40
  int resPin = SPIN_TFT_RES;
#endif
#ifdef ARDUINO_TEENSY41
  int resPin = PIN_TFT_RES;
#endif
  digWrite(resPin, HIGH);
  delay(5);
  digWrite(resPin, LOW);
  delay(20);
  digWrite(resPin, HIGH);
  delay(150);
}

void digWrite(int pin, uint8_t stat)
{
  if(pin & 0x8000)
    ;//   portExpSetBit(pin & 0x00FF, stat);  //@@@
  else
    digitalWrite(pin, stat);
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
#ifdef ARDUINO_TEENSY41
  if(is12V())
    return InternalTemperature.readTemperatureC() - TEMP_OFFS_STATIONARY;
  else
    return InternalTemperature.readTemperatureC() - TEMP_OFFS_PORTABLE;
#endif
#ifdef ARDUINO_TEENSY40
  return 22;  //TODO
#endif
}

void powerOff()
{
#ifdef ARDUINO_TEENSY41
  digitalWrite(PIN_POWER_OFF, 0);
#endif
#ifdef ARDUINO_TEENSY40
  digWrite(SPIN_POWER_OFF, 0);
#endif
}

bool hasDisplay()
{
#ifdef ARDUINO_TEENSY40
  return (digitalRead(PIN_REV0) == 0);
#endif
#ifdef ARDUINO_TEENSY41
  return true;
#endif
}


void setDispLight(uint8_t bright)
{
#ifdef ARDUINO_TEENSY40
  if(hasDisplay())
  {
    if(bright > 128)
      digWrite(SPIN_LED_DISP, 1);
    else
      digWrite(SPIN_LED_DISP, 0);
  }
#endif
#ifdef ARDUINO_TEENSY41  
  uint8_t pinLed;
  else if(isRevisionB())
    pinLed = PIN_TFT_LED_REVB;
  else
    pinLed = PIN_TFT_LED_REVA;
  if(bright > 128)
    digitalWrite(pinLed, 1);
  else
    digitalWrite(pinLed, 0);
#endif  
}


#ifdef ARDUINO_TEENSY40
void portExpSetBit(uint8_t port, uint8_t state)
{
  if(state == 0)
    ioexOut &= ~port;
  else
    ioexOut |= port;
  ioex.output(ioexOut);
}
#endif