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
#include "Adafruit_SSD1327.h"
#include "Adafruit_ILI9341.h"

#include "debug.h"
#include "cmenue.h"
#include "cMeanCalc.h"
#include "InternalTemperature.h"
#include "globals.h"


cMeanCalc<int32_t,50> digits;
//cMeanCalc<int16_t,10> uref;

uint8_t ioexOut = 0;
bool ioDebugMode = false;

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
 // pinMode(SPIN_LED_DISP, OUTPUT);
 // pinMode(SPIN_LED_2, OUTPUT); 

  Wire.begin();
  ioex.attach(Wire);
  bool ok = sht.init();
  if(!ok)
    Serial.println("error initializing humidity sensor");
  sht.setAccuracy(SHTSensor::SHTAccuracy::SHT_ACCURACY_HIGH);
  ioex.setDeviceAddress(I2C_ADDR_PORT_EXT);
  ioex.config(TCA9534::Config::OUT); // set all port to output
  ioex.polarity(TCA9534::Polarity::POL_ORIGINAL); // set all port polarity to original
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

Adafruit_ILI9341 tft = Adafruit_ILI9341(PIN_TFT_CS, PIN_TFT_DC_REVA,
                            PIN_TFT_MOSI, PIN_TFT_SCLK, PIN_TFT_MISO);
Adafruit_SSD1327 oled(128, 128, &Wire, -1, 400000, 100000);

const MEMP stColors TftColors
{
  COL_TFT_TEXT,         //uint16_t text;
  COL_TFT_TEXT_PAR,     //uint16_t textPar;
  COL_TFT_TEXT_DIR,     //uint16_t textDir;
  COL_TFT_TEXTBACK,     //uint16_t textBack;
  COL_TFT_TEXTSEL,      //uint16_t textSel;
  COL_TFT_TEXTSELBACK,  //uint16_t textSelBack;
  COL_TFT_TEXTEDIT,     //uint16_t textEdit;
  COL_TFT_TEXTEDITBACK, //uint16_t textEditBack;
  COL_TFT_MSGSHADOW,    //uint16_t msgShadow;
  COL_TFT_TEXTHDR,      //uint16_t textHdr;
  COL_TFT_TEXTHDRBACK,  //uint16_t textHdrBack;
  COL_TFT_TEXTDROPBACK, //uint16_t textDropBack;
  COL_TFT_MENULINE,     //uint16_t menuLine
  COL_TFT_GRID,         //uint16_t grid;
  COL_TFT_GRAPH,        //uint16_t graph;
  COL_TFT_CURSOR        //uint16_t cursor;
};

const MEMP stColors OledColors
{
  COL_OLED_TEXT,         //uint16_t text;
  COL_OLED_TEXT_PAR,     //uint16_t textPar;
  COL_OLED_TEXT_DIR,     //uint16_t textDir;
  COL_OLED_TEXTBACK,     //uint16_t textBack;
  COL_OLED_TEXTSEL,      //uint16_t textSel;
  COL_OLED_TEXTSELBACK,  //uint16_t textSelBack;
  COL_OLED_TEXTEDIT,     //uint16_t textEdit;
  COL_OLED_TEXTEDITBACK, //uint16_t textEditBack;
  COL_OLED_MSGSHADOW,    //uint16_t msgShadow;
  COL_OLED_TEXTHDR,      //uint16_t textHdr;
  COL_OLED_TEXTHDRBACK,  //uint16_t textHdrBack;
  COL_OLED_TEXTDROPBACK, //uint16_t textDropBack;
  COL_OLED_MENULINE,     //uint16_t menuLine
  COL_OLED_GRID,         //uint16_t grid;
  COL_OLED_GRAPH,        //uint16_t graph;
  COL_OLED_CURSOR        //uint16_t cursor;
};


void initDisplay()
{
  switch (hasDisplay())
  {
    case enDisplayType::OLED_128:
      pDisplay = &oled;
      oled.begin(0x3C);
      oled.clearDisplay();
      oled.display();
      menue.setPdisplay(DISP_HEIGHT_OLED, DISP_WIDTH_OLED, pDisplay, LINE_HEIGHT_OLED, 2, &OledColors);
      break;
    case enDisplayType::TFT_320:
      pDisplay = &tft;
      tft.begin();
      menue.setPdisplay(DISP_WIDTH_TFT, DISP_HEIGHT_TFT, pDisplay, LINE_HEIGHT_TFT, 4, &TftColors);
      break;
  }
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
    if(hasDisplay() != enDisplayType::NO_DISPLAY)
    {
      setDispLight(1);
      menue.showMsg(enMsg::INFO, nullptr, Txt::get(2100));
      delay(500);
    }
    powerOff();
  }    
}

float readSupplyVoltage()
{
  float volt = 0;
  int32_t digSingle = analogRead(PIN_SUPPLY_VOLT);
  int digs = digits.get(digSingle);
  devStatus.digits.set((float)digs);

  volt = (float)digs * devPars.voltFactor.get();

  DPRINTF2("get Voltage: digits: %i single: %i  voltage: %f  factor: %f\n", digs, digSingle, volt,devPars.voltFactor.get());
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
  if(!ioDebugMode)
  {
    if(pin & 0x8000)
      portExpSetBit(pin & 0x00FF, stat);
    else
      digitalWrite(pin, stat);
  }
}

void setIoDebugMode(bool mode)
{
  ioDebugMode = mode;
}

float calcVoltageFactor(float volt)
{
  float fact = 1.0;
  int32_t digits = 0;
  int cnt = 50;
  for(int i = 0; i < cnt; i++)
  {
    digits += analogRead(PIN_SUPPLY_VOLT);
    delay(2);
  }
  digits /= cnt;
  
  if(digits > 0)
  {
      fact = volt / digits;
  }
  DPRINTF2("calc factor: digits: %i, voltage: %f, factor: %f\n", digits, volt, fact);
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
  sht.readSample();
  return sht.getTemperature();
#endif
}

void blink(int cnt)
{
  if(hasDisplay() == enDisplayType::NO_DISPLAY)
  {
    for(int i = 0; i < cnt; i++)
    {
      digWrite(SPIN_LED_2, 1);
      digWrite(SPIN_LED_DISP, 1);
      delay(100);
      digWrite(SPIN_LED_2, 0);
      digWrite(SPIN_LED_DISP, 0);
      delay(100);
    }
  }
  else
    delay(200 * cnt);
}

void powerOff()
{
  blink(5);
  if((hasDisplay() == enDisplayType::NO_DISPLAY) && !wheels.isKeyPressed())
    return;

  audio.closeProject();
  delay(300);
  blink(3);
  sysLog.close();
  gpsLog.close();
  cSdCard::inst().unmount();
  delay(300);
  blink(3);
#ifdef ARDUINO_TEENSY41
  digitalWrite(PIN_POWER_OFF, 0);
#endif
#ifdef ARDUINO_TEENSY40
  digWrite(SPIN_POWER_OFF, 0);
#endif
  for(;;)  { }
}

int hasDisplay()
{
#ifdef ARDUINO_TEENSY40
  if (digitalRead(PIN_REV0) == 0)
    return enDisplayType::OLED_128;
  else if (digitalRead(PIN_REV1) == 0)
    return enDisplayType::TFT_320;
  else
    return enDisplayType::NO_DISPLAY;
#endif
#ifdef ARDUINO_TEENSY41
  return enDisplyType::TFT_320;
#endif
}


void setDispLight(uint8_t bright)
{
#ifdef ARDUINO_TEENSY40
  if(hasDisplay() != enDisplayType::NO_DISPLAY)
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