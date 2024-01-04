/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

//#define DEBUG_LEVEL 4
#include "debug.h"
#include "fnt8x11.h"
#include "debug.h"
//#include "clFixMemPool.h"
#include "pnlmain.h"
#include "cSdCard.h"
#include "pnllive.h"
#include "pnlparams.h"
#include "globals.h"
#include "cEeprom.h"

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
// *********************** initialization **************************
void initTft(int orientation)
{
  if(hasDisplay())
  {
    if(isRevisionB())
      tft = ILI9341_t3(PIN_TFT_CS, PIN_TFT_DC_REVA, 
                     PIN_TFT_MOSI, PIN_TFT_SCLK, PIN_TFT_MISO);
    resetTft();
    tft.begin();
    tft.setRotation(orientation);
    tft.fillScreen(ILI9341_BLACK);
    tft.setTextColor(ILI9341_YELLOW);
    tft.setTextSize(3);
    tft.setFont(fnt8x11);  
    setDispLight(255);
    showSplashScreen(tft, true);
  }
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
  delay(200);
  initPins();
  if(!hasDisplay())
  {
    digWrite(SPIN_LED_DISP, 1);
    digWrite(SPIN_LED_2, 1);
  }
  //waitForSerial();
  audio.init();
  Txt::setResource(Texts);
  int orientation = readInt16FromEep(0x0032) == 0 ? 3 : 1;
  initTft(orientation);
  cSdCard::inst().mount();
  sysLog.log("power on");
  delay(500);  
  menue.init(hasDisplay());
  menue.initFileRelatedParams();
 // tft.setRotation(devPars.dispOrient.get() == 0 ? 3 : 1);
  menue.refreshAll();
  menue.printPars();
  getSerialNr(serialNumber, sizeof(serialNumber));
  audio.setup();
  wheels.setDirection(true);  
  if(hasDisplay())
  {
    setDispLight(255);
    setVisibilityRecCount(&menue);
  }
  else
  {
    digWrite(SPIN_LED_DISP, 0);
    digWrite(SPIN_LED_2, 0);
  }

  devStatus.opMode.set(enOpMode::HEAR_HET);
  gps.init();
  if(devPars.recAuto.get() == 3)
    calcSunrise();
}


void handleDisplayAndWheel(bool oneSec)
{
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
      terminal.parseCmdfromUSB();

    enKey key = terminal.getKey();
    if (key == NOKEY)
      key = wheels.getKey();

    //handle commands
    if(key != enKey::NOKEY)
    {      
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
  }
  if(oneSec)
  {
    devStatus.cpuAudioAvg.set(AudioProcessorUsage());
    devStatus.cpuAudioMax.set(AudioProcessorUsageMax());
    devStatus.audioMem.set(AudioMemoryUsage());
    size_t freeSpace;  size_t totSpace;
    cSdCard::inst().getFreeMem(freeSpace, totSpace);
    devStatus.freeSpace.set(freeSpace / 1024);
  }
}

cTimer switchOff;
cTimer function;
bool functionExecuted = false;

void handleButtonsAndLeds()
{
  if (!cSdCard::inst().isFileTransferActive())
  {
    bool res = terminal.parseCmdfromUSB();
    if(res)
    {
      audio.setup();
    }
  }
  if(wheels.getKey() == enKey::KEY_RELEASED)
  { 
    if(functionExecuted)
      functionExecuted = false;
    else
      statusDisplay.nextState();
  }
  if(wheels.isKeyPressed())
  { 
    if(!switchOff.isRunning())
      switchOff.setAlarm_ms(3000);
    if(!function.isRunning() && !functionExecuted)
      function.setAlarm_ms(500);
  }
  else
  {
    switchOff.stop();
    function.stop();
  }
  
  if(switchOff.isAlarm())
  {
    audio.stopRecording();
    powerOff();
  }
  if(function.isAlarm())
  {
    functionExecuted = true;
    function.stop();
    statusDisplay.execFunction();
  }
  statusDisplay.show();
}


// *********************** main loop **************************
int loopCount = 0;

void loop() 
{
  loopCount++;
  
  bool rtFft;
  if(hasDisplay())
    rtFft = (menue.getFocusPanel() == pnlLive) ||
              ((menue.getMainPanel() == pnlLive) && (menue.getFocusPanel() == menue.getFkeyPanel())); 
  else
    rtFft = terminal.isOnline();
  audio.operate( rtFft );
  audio.checkAutoRecording(menue, rtc);
  
  bool tickOneSec = tick1s.check();

  if (hasDisplay())
    handleDisplayAndWheel(tickOneSec);
  else
    handleButtonsAndLeds();

  if(tickOneSec)
  {
    if(devPars.srcPosition.get() != enPositionMode::POS_FIX)
    {
      float lat, lon, altitude;
      gps.operate(lat, lon, altitude);
      devStatus.geoPos.set(lat, lon);
    }

    devStatus.mainLoop.set(loopCount);
    devStatus.peakVal.set(audio.getLastPeakVal() * 100);
    loopCount = 0;
    float volt = readSupplyVoltage();
    devStatus.voltage.set(volt);
    float temp = readTemperature();
    devStatus.temperature.set(temp);
    //  if(hasDisplay())
    //    /*cParGraph* g =*/ getLiveFft();
    devStatus.time.set(rtc.getTime());
    devStatus.date.set(rtc.getTime());
    devStatus.time.update(true);
  }

  if(tick15Min.check())
  {
    if(devPars.recAuto.get() == enRecAuto::TWILIGHT)
      calcSunrise();
    sysLog.logf("supply voltage: %2.3f V, temperature: %2.1f °C", 
               devStatus.voltage.get(), devStatus.temperature.get());
    checkSupplyVoltage();
  }
}


