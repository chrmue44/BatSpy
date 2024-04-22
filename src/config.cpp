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
#include "chmfont.h"
#include "fnt8x11.h"
#include "cutils.h"
#include "globals.h"
#include "startup_pic.c_"

#if defined(__IMXRT1062__)
extern "C" uint32_t set_arm_clock(uint32_t frequency);
#endif


cMeanCalc<int32_t,50> digits;

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
  initTempSensor();
  ioex.setDeviceAddress(I2C_ADDR_PORT_EXT);
  ioex.config(TCA9534::Config::OUT); // set all port to output
  ioex.polarity(TCA9534::Polarity::POL_ORIGINAL); // set all port polarity to original
  ioex.output(ioexOut);
  digWrite(SPIN_LED_2, 1);
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
strncpy(buf, "RevC", sizeof(buf));
#endif
  devStatus.hwVersion.set(buf);
}

Adafruit_ILI9341 tft = Adafruit_ILI9341(PIN_TFT_CS, PIN_TFT_DC_REVA,
                            PIN_TFT_MOSI, PIN_TFT_SCLK, PIN_TFT_MISO);
Adafruit_SSD1327 oled(128, 128, &Wire, -1, 400000, 100000);

const stColors PROGMEM TftColors
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

const stColors PROGMEM OledColors
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

void MEMP showSplashScreen(Adafruit_GFX& tft, bool waitBtn)
{
  char buf[20];
  if (hasDisplay() == enDisplayType::OLED_128)
  {
    oled.clearDisplay();
    oled.setTextColor(SSD1327_WHITE);
    oled.drawRect(2, 2, DISP_WIDTH_OLED - 2, DISP_HEIGHT_OLED - 2, COL_OLED_TEXTHDRBACK);
    oled.fillRect(3, 3, DISP_WIDTH_OLED - 4, DISP_HEIGHT_OLED - 4, COL_OLED_TEXTDROPBACK);

    oled.setCursor(45, 10);
    oled.print("BatSpy");
    oled.setCursor(1, 25);
    oled.print(Txt::get(430));
    oled.setCursor(1, 34);
    oled.print(devStatus.version.get());
    oled.setCursor(1, 48);
    oled.print(Txt::get(435));
    oled.setCursor(1, 57);
    oled.print(devStatus.hwVersion.get());
    memset(buf, 0, sizeof(buf));
    getSerialNr(buf, sizeof(buf));
    oled.setCursor(1, 71);
    oled.print(Txt::get(437));
    oled.setCursor(1, 80);
    oled.print(buf);

    oled.setCursor(1, 99);
    oled.print("(C) 2021..24");
    oled.setCursor(1, 108);
    oled.print("Christian M" CH_UEs "ller");
    oled.setCursor(1, 125);
    oled.print(Txt::get(1704));
    oled.display();
  }
  else
  {
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_WHITE);
    tft.drawRect(2, 2, DISP_WIDTH_TFT - 2, DISP_HEIGHT_TFT - 2, ILI9341_YELLOW);
    tft.fillRect(3, 3, DISP_WIDTH_TFT - 4, DISP_HEIGHT_TFT - 4, COL_TFT_TEXTDROPBACK);

    tft.setCursor(140, 10);
    tft.print("BatSpy");
    tft.setCursor(30, 25);
    tft.print(Txt::get(1700));
    //tft.writeRect(96,55,128, 128, startup_pic);
    tft.drawRGBBitmap(96, 55, startup_pic, 128, 128);
    tft.setCursor(30, 195);
    tft.print(Txt::get(1702));
    tft.print(devStatus.version.get());
    tft.setCursor(30, 210);
    tft.print("(C) 2021..23 Christian M" CH_UEs "ller");
    tft.setTextColor(ILI9341_LIGHTGREY);
    tft.setCursor(140, 225);
    tft.print(Txt::get(1704));
  }

  bool exit = false;
  if (waitBtn)
  {
    do
    {
      exit = !digitalRead(PIN_ROT_LEFT_S);
      yield();
    } while (!exit);
  }
  else
    delay(1000);
  if (hasDisplay() == enDisplayType::TFT_320)
    tft.fillScreen(ILI9341_BLACK);
  else
    oled.clearDisplay();
}

void testDisplay()
{
  int x = 44, y = 2;
  int state = 0;
  int r = 5;
  pDisplay->fillRect(0, 0, pDisplay->width(), pDisplay->height(), 0xFF);
  oled.setTextColor(SSD1327_BLACK);
  oled.setCursor(30, 50);
  oled.print("Display test");
  oled.setCursor(30, 65);
  oled.print("stop with OK");
  
  for(;;)
  { 
    pDisplay->fillRect(x-2, y-2, 4*r, 4*r, 0xFF);
    switch (state)
    {
    case 0:
      if (x < (pDisplay->width() - 2 * r - 2))
        x += 1;
      else
        state = 1;
        break;
    case 1:
      if (y < (pDisplay->height() - 2* r - 2))
        y += 1;
      else
        state = 2;
        break;
    case 2:
      if (x > 1)
        x -= 1;
      else
        state = 3;
        break;
    case 3:
      if (y > 1)
        y -= 1;
      else
        state = 0;
      break;
    }
    pDisplay->fillCircle(x+r, y+r, r, 0);
    oled.display();
    if( !digitalRead(PIN_ROT_LEFT_S))
    {
      oled.clearDisplay();
      break;
    }
    delay(25);
  }
}


void initDisplay(int orientation)
{
  enDisplayType dType = (enDisplayType)hasDisplay();
  if (dType != enDisplayType::NO_DISPLAY)
  {
    if (dType == enDisplayType::TFT_320)
    {
      pDisplay = &tft;
      tft.begin();  //TODO
      menue.setPdisplay(DISP_WIDTH_TFT, DISP_HEIGHT_TFT, pDisplay, LINE_HEIGHT_TFT, 4, &TftColors, 0);
//      if (isRevisionB())
//        tft = ILI9341_t3(PIN_TFT_CS, PIN_TFT_DC_REVA,
//          PIN_TFT_MOSI, PIN_TFT_SCLK, PIN_TFT_MISO);
      resetTft();
      tft.begin();
      tft.setRotation(orientation);
      tft.fillScreen(ILI9341_BLACK);
      tft.setTextColor(ILI9341_YELLOW);
      tft.setTextSize(3);
//      tft.setFont(fnt8x11);
    }
    if (dType == enDisplayType::OLED_128)
    {
      pDisplay = &oled;
      oled.begin(0x3C);
      oled.cp437(true);
      oled.setFont(&chmFont);
      oled.clearDisplay();
      oled.display();
      menue.setPdisplay(DISP_HEIGHT_OLED, DISP_WIDTH_OLED, pDisplay, LINE_HEIGHT_OLED, 2, &OledColors, 7);
    }
    setDispLight(255);
    //testDisplay();
    showSplashScreen(tft, true);
    resetTft();
    pDisplay->setRotation(orientation);
    pDisplay->fillScreen(SSD1327_BLACK);
    pDisplay->setTextColor(SSD1327_WHITE);
    pDisplay->setTextSize(3);
    setDispLight(255);
  }
}


enCpuMode cpuMode = enCpuMode::CPU_MODE_INIT;
void setHwOperationMode(enCpuMode mode)
{
  if(cpuMode != mode)
  {
    switch(mode)
    {
      case enCpuMode::POWER_SAVE:
        audio.enable(false);
        setAnalogPower(false);
        set_arm_clock(CPU_FREQ_LOW);
        sysLog.log("enable power save mode");
        break;
      case enCpuMode::RECORDING:
        set_arm_clock(CPU_FREQ_HIGH);
        audio.enable(true);
        setAnalogPower(true);
        sysLog.log("enable recording mode");
        break;
      case enCpuMode::CPU_MODE_INIT:
        break;
    }
    cpuMode = mode;
    DPRINTF1("CPU Freq: %i\n", F_CPU_ACTUAL);
  }
}

void setAnalogPower(bool on)
{
#ifdef ARDUINO_TEENSY40
  digWrite(SPIN_PWR_ANA, on ? 1 : 0);
#endif
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
      setDispLight(255);
      menue.showMsg(enMsg::INFO, nullptr, hasDisplay() == enDisplayType::OLED_128, Txt::get(2100));
      delay(1000);
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

void initTempSensor()
{
  sht.begin(Wire, SHT40_I2C_ADDR_44);
  sht.softReset();
  delay(10);
  uint32_t serialNumber = 0;
  int error = sht.serialNumber(serialNumber);
  if (error != 0) 
  {
    sysLog.log("attempt to init temp sensor I2S addr 0x44 failed");
    sht.begin(Wire, SHT40_I2C_ADDR_45);
    sht.softReset();
    delay(10);
    error = sht.serialNumber(serialNumber);
    if (error != 0)
    { 
      sysLog.log("attempt to init temp sensor I2S addr 0x44 failed");
      return;
    }
  }
  sysLog.log("temp sensor successfully initialized");
}

float readTemperature(float& humidity)
{
#ifdef ARDUINO_TEENSY41
  if(is12V())
    return InternalTemperature.readTemperatureC() - TEMP_OFFS_STATIONARY;
  else
    return InternalTemperature.readTemperatureC() - TEMP_OFFS_PORTABLE;
  humidity = 0.0;
#endif
#ifdef ARDUINO_TEENSY40
  float t;
  int err = sht.measureHighPrecision(t, humidity);
  if(err != 0)
  {
    t = NAN;
    humidity = NAN;
  }
  return t;
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
  if (hasDisplay() == enDisplayType::OLED_128)
  {
    oled.clearDisplay();
    oled.setCursor(5, 40);
    oled.print(Txt::get(2100));
    oled.display();
  }
  audio.closeProject();
  delay(300);
  blink(3);
  sysLog.close();
  gpsLog.close();
  trigLog.close();
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
  
  switch(hasDisplay())
  {
    case enDisplayType::TFT_320:
      if(bright > 128)
        digWrite(SPIN_LED_DISP, 1);
      else
        digWrite(SPIN_LED_DISP, 0);
      break;
    case enDisplayType::OLED_128:
      if(bright > 128)
      {
        menue.enable(true);
        menue.refreshAll();
      }
      else
      {
        oled.clearDisplay();
        oled.display();
        menue.enable(false);
      }

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
  uint8_t oldState = ioexOut;
  if(state == 0)
    ioexOut &= ~port;
  else
    ioexOut |= port;
  if(oldState != ioexOut)
    ioex.output(ioexOut);
}
#endif