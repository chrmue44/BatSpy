/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/
#include <Arduino.h>
//#define DEBUG_LEVEL 4
#include "debug.h"
#include "config.h"
#include "fnt8x11.h"
///#include "clFixMemPool.h"
#include "pnlmain.h"
#include "cSdCard.h"
///#include "pnllive.h"
///#include "pnlparams.h"
#include "globals.h"
#include "ctext.h"

extern struct stTxtList Texts[];
/*
const tChunkTab memChunks[] = {
  {1024, 10},
  {512, 10},
  {256, 10},
  {128, 10},
  {64, 20},
  {32, 20},
  {16, 20},
  {8, 20}
};
*/
//clFixMemPool* mem = clFixMemPool::getInstance(memChunks, sizeof(memChunks)/sizeof(memChunks[0]));

void setDispLight(uint8_t bright)
{
  if(bright > 128)
    digitalWrite(PIN_TFT_LED, 1);
  else
    digitalWrite(PIN_TFT_LED, 0);  
}

// *********************** initialization **************************
void initTft()
{
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(3);
  tft.setFont(fnt8x11);  
  setDispLight(255);
}

void waitForSerial()
{
  int a = 0;
  while( a == 0)
  {
    Serial.println("press key to coninue");
    a = Serial.available();
    delay(500);
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("setting up bat detector");
  initPins();
  ///audio.init();
//  waitForSerial();
  Txt::setResource(Texts);
  setDispLight(255);
  initTft();
  showSplashScreen(tft, digitalRead(PIN_ID_12V) == 1);
  cSdCard::inst().mount();
  ///sysLog.log("power on");
  delay(500);  
  /**menue.init();
  tft.setRotation(devPars.dispOrient.get() == 0 ? 3 : 1);
  menue.refreshAll();
  menue.printPars();
  **/
  ///audio.setup();
  ///wheels.setDirection(true);  
  setDispLight(255);
  /** devStatus.opMode.set(enOpMode::HEAR_HET);
  setVisibilityRecCount(&menue);
  **/
  ///gps.init();
  /**
  if(devPars.recAuto.get() == 3)
    calcSunrise();
   **/
}


// *********************** main loop **************************
int loopCount = 0;
int loopFreq;
void loop() 
{
  static int i;
  if(i > 128)
    i = 0;
  else
    i = 255;
  delay(1000);
  setDispLight(i);
  Serial.print(i);
}
/*
  loopCount++;
  static bool backLightOn = true;
  if (tick300ms.check())
  {
    menue.handleKey(enKey::TICK);
    if (menue.keyPauseLongEnough(devPars.backLightTime.get() * 1000)) 
    {
      if(backLightOn)
      {
        setDispLight(0);
        backLightOn = false;
      }
    } 
  }
  else
  {
    // get new commands
    if (!cSdCard::inst().isFileTransferActive())
      terminal.parseCmd();
    enKey key = terminal.getKey();
    if (key == NOKEY)
      key = wheels.getKey();

    //handle commands
    if(key != enKey::NOKEY)
    {
      // just update time and date here to minimize noise
/*      devStatus.time.set(rtc.getTime());
      devStatus.date.set(rtc.getTime());
      devStatus.time.update(true); */
      
/**      if(!backLightOn)
      {
        backLightOn = true;
        setDispLight(255);
        menue.resetTimer();
      }
      else
      {
        menue.handleKey(key);
        audio.setup();
        audio.updateCassMode();
        tft.setRotation(devPars.dispOrient.get() == 0 ? 3 : 1);
        wheels.setDirection(devPars.knobRotation.get() == 0);
      }
    }

    //handle audio processing
    audio.checkAutoRecording(menue, rtc);

    if(tick1s.check())
    {
      if(devPars.srcPosition.get() == 1)
      {
        float lat, lon;
        gps.operate(lat, lon);
        devStatus.geoPos.set(lat, lon);
      }

      loopFreq = loopCount;
      loopCount = 0;
      devStatus.cpuAudioAvg.set(AudioProcessorUsage());
      devStatus.cpuAudioMax.set(AudioProcessorUsageMax());
      devStatus.audioMem.set(AudioMemoryUsage());
      devStatus.peakVal.set(audio.getLastPeakVal() * 100);
      size_t freeSpace;  size_t totSpace;
      cSdCard::inst().getFreeMem(freeSpace, totSpace);
      float volt = readSupplyVoltage();
      devStatus.voltage.set(volt);
      devStatus.freeSpace.set(freeSpace / 1024);
      float temp = readTemperature();
      devStatus.temperature.set(temp);
      cParGraph* g = getLiveFft();
      devStatus.msPerDiv.set(g->getMsPerDiv());
      devStatus.time.set(rtc.getTime());
      devStatus.date.set(rtc.getTime());
      devStatus.time.update(true);
    }

    if(tick15Min.check())
    {
      if(devPars.recAuto.get() == enRecAuto::TWILIGHT)
        calcSunrise();
      sysLog.logf("supply voltage: %2.1f V, temperature: %2.1f °C", 
                 devStatus.voltage.get(), devStatus.temperature.get());
      checkSupplyVoltage();
    }
  }
  bool rtFft = (menue.getFocusPanel() == pnlLive) ||
              ((menue.getMainPanel() == pnlLive) && (menue.getFocusPanel() == menue.getFkeyPanel())); 
  audio.operate( rtFft );
} **/
