/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

//#define DEBUG_LEVEL 4
#include "debug.h"
#include "cAudio.h"
#include "config.h"
#include <ILI9341_t3.h>
#include <Metro.h>
#include "fnt8x11.h"
#include "cmenue.h"
#include "cRtc.h"
#include "cWheels.h"
#include "debug.h"
#include "cTerminal.h"
#include "clFixMemPool.h"
#include "pnlmain.h"
#include "cSdCard.h"
#include "cLog.h"
#include "pnllive.h"
#include "pnlparams.h"

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
// ************************ global objects *************************
//clFixMemPool* mem = clFixMemPool::getInstance(memChunks, sizeof(memChunks)/sizeof(memChunks[0]));
ILI9341_t3 tft = ILI9341_t3(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST, 
                            PIN_TFT_MOSI, PIN_TFT_SCLK, PIN_TFT_MISO);
cAudio audio;  // audio control
cRtc rtc;
cMenue menue(320, 240, &tft);
Metro tick300ms(300);
Metro tick1s(1000);
Metro tick15Min(1000 * 60 * 15);

cWheels wheels(PIN_ROT_LEFT_A, PIN_ROT_LEFT_B, PIN_ROT_LEFT_S,
               PIN_ROT_RIGHT_A, PIN_ROT_RIGHT_B, PIN_ROT_RIGHT_S);
cTerminal terminal;

  void setDispLight(uint8_t bright)
  {
#ifdef ARDUINO_TEENSY41
  if(bright > 128)
    digitalWrite(PIN_TFT_LED, 1);
  else
    digitalWrite(PIN_TFT_LED, 0);  
#else
  analogWrite(PIN_TFT_LED, bright);
#endif  
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
  initPins();
  audio.init();
  Serial.begin(9600);
  Serial.println("setting up bat detector");
//  waitForSerial();
  Txt::setResource(Texts);
  initTft();
  showSplashScreen(tft, digitalRead(PIN_ID_12V) == 1);
  cSdCard::inst().mount();
  cLog::log("power on");
  delay(500);  
  menue.init();
  tft.setRotation(devPars.dispOrient.get() == 0 ? 3 : 1);
  menue.refreshAll();
  menue.printPars();
  audio.setup();
  wheels.setDirection(true);  
  setDispLight(255);
  devStatus.opMode.set(enOpMode::HEAR_HET);
  setVisibilityRecCount(&menue);
}


// *********************** main loop **************************
int loopCount = 0;
int loopFreq;
void loop() 
{
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
      devStatus.time.set(rtc.getTime());
      devStatus.date.set(rtc.getTime());
      devStatus.time.update(true);
      
      if(!backLightOn)
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
    }

    if(tick15Min.check())
    {
      if(devPars.recAuto.get() == enRecAuto::TWILIGHT)
        calcSunrise();
      cLog::logf("supply voltage: %2.1f V, temperature: %2.1f °C", 
                 devStatus.voltage.get(), devStatus.temperature.get());
      checkSupplyVoltage();
    }
  }
  bool rtFft = (menue.getFocusPanel() == pnlLive) ||
              ((menue.getMainPanel() == pnlLive) && (menue.getFocusPanel() == menue.getFkeyPanel())); 
  audio.operate( rtFft );
}

