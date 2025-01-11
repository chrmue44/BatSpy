/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

//#define DEBUG_LEVEL 4
#include "debug.h"
#include "Adafruit_SSD1327.h"
#include "debug.h"
//#include "clFixMemPool.h"
#include "pnlmain.h"
#include "cSdCard.h"
#include "pnllive.h"
#include "pnlparams.h"
#include "globals.h"
#include "cEeprom.h"
#include "cBattery.h"

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

void logStatus()
{
  devStatus.voltage.set(readSupplyVoltage());
  devStatus.chargeLevel = cBattery::getChargeCondition(devStatus.voltage.get());
  sysLog.logf("supply voltage: %2.3f V, Battery %.0f %%, temperature: %2.1f °C, Humidity: %.0f\n", 
               devStatus.voltage.get(), devStatus.chargeLevel.get(), devStatus.temperature.get(), devStatus.humidity.get());
}


void initDisplayVars()
{
  size_t freeSpace;  size_t totSpace;
  cSdCard::inst().getFreeMem(freeSpace, totSpace);
  devStatus.freeSpace.set(freeSpace / 1024/1024);
  float humidity;
  float temp = readTemperature(humidity);
  devStatus.temperature.set(temp);
  devStatus.humidity.set(humidity);
  devStatus.voltage.set(readSupplyVoltage());
  devStatus.chargeLevel = cBattery::getChargeCondition(devStatus.voltage.get());
  devStatus.battSymbol.set(cBattery::getBatterySymbol(devStatus.chargeLevel.get()));
}

void setup()
{
  size_t parSet = PARS_BAT;

  Serial.begin(9600);
  delay(200);
  initPins();
  if(hasDisplay() == enDisplayType::NO_DISPLAY)
  {
    digWrite(SPIN_LED_DISP, 1);
    digWrite(SPIN_LED_2, 1);
  }
//  waitForSerial();
  audio.init();
  Txt::setResource(Texts);
  int orientation = readInt16FromEep(EEPADDR_DISP_ORIENT) == 0 ? 0 : 2;
  bool dispModeInv = readInt16FromEep(EEPADDR_DISP_MODE) != 0;
  initDisplay(orientation, 255, dispModeInv, true);
  enSdRes res = cSdCard::inst().mount();
   char serial[16];
   getSerialNr(serial, sizeof(serial));
  delay(500);
  sysLog.log("power on");
  sysLog.logf("Serial Nr: %s, Software Version: %s\n",serial, devStatus.version.get());
  if(res != enSdRes::OK)
    sysLog.log("mounting of SD card failed");
  menue.init(hasDisplay() != enDisplayType::NO_DISPLAY);
  menue.initFileRelatedParams();
  // tft.setRotation(devPars.dispOrient.get() == 0 ? 3 : 1);
  menue.refreshAll();
  menue.printPars(parSet);
  getSerialNr(serialNumber, sizeof(serialNumber));
  audio.setup();
  wheels.setDirection(true);
  if(hasDisplay() != enDisplayType::NO_DISPLAY)
  {
    setBackLight(true);
    setVisibilityRecCount(&menue);
    menue.resetTimer();
  }
  else
  {
    digWrite(SPIN_LED_DISP, 0);
    digWrite(SPIN_LED_2, 0);
  }

  devStatus.opMode.set(enOpMode::HEAR_HET);
  gps.init();
  gps.setMode((enGpsMode)devPars.srcPosition.get());
  if(devPars.recAuto.get() == 3)
    calcSunrise();
  initDisplayVars();
  logStatus();
  digWrite(SPIN_LED_2, 0);
  micInfo.read();
  Serial.printf("Microphone: %s\n",micInfo.getId());
}


void handleDisplayAndWheel(bool oneSec)
{
  bool rtFft;
  if(hasDisplay() == enDisplayType::TFT_320)
    rtFft = (menue.getFocusPanel() == pnlLive) ||
              ((menue.getMainPanel() == pnlLive) && (menue.getFocusPanel() == menue.getFkeyPanel()));
  else
    rtFft = terminal.isOnline();
  audio.operate( rtFft);
  if (tick300ms.check())
  {
    menue.handleKey(enKey::TICK);
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
      if(!isBackLightOn())
      {
        setBackLight(true);
        menue.resetTimer();
//        command.addToQueue(enCmd::BACKLIGHT_ON);
      }
      else
      {
        menue.handleKey(key);
        audio.setup();
        audio.updateCassMode();
//        tft.setRotation(devPars.dispOrient.get() == 0 ? 3 : 1);
        pDisplay->setRotation(devPars.dispOrient.get() == 0 ? 0 : 2);
        wheels.setDirection(devPars.knobRotation.get() == 0);
      }
    }
  }
  if(oneSec)
  {
    devStatus.cpuAudioAvg.set(AudioProcessorUsage());
    devStatus.cpuAudioMax.set(AudioProcessorUsageMax());
    devStatus.audioMem.set(AudioMemoryUsage());
  }
}

cTimer switchOff;
cTimer function;
bool functionExecuted = false;
const char* MSG_PWR_DN_BUTTON = "power down via button";

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
	command.addToQueue(enCmd::LOG, (void*)MSG_PWR_DN_BUTTON);
	command.addToQueue(enCmd::POWER_OFF);
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
extern void stateMachine();
void loop()
{
  loopCount++;

  bool tickOneSec = tick1s.check();

  if (hasDisplay() != enDisplayType::NO_DISPLAY)
    handleDisplayAndWheel(tickOneSec);
  else
    handleButtonsAndLeds();

  stateMachine();

  bool gpsValid = false;
  if(devPars.srcPosition.get() != static_cast<uint32_t>(enPositionMode::FIX))
  {
    gpsValid = gps.operate();
  }
  if(tickOneSec)
  {
    if((devPars.srcPosition.get() != static_cast<uint32_t>(enPositionMode::FIX)) &&
        (gps.getStatus() == enGpsStatus::GPS_SEARCHING))
       digWrite(SPIN_LED_2, 1);
    else
       digWrite(SPIN_LED_2, 0);
    if(gpsValid)
    {
      devStatus.geoPos.set(gps.getLat(), gps.getLon());
      devStatus.lonSign.set(devStatus.geoPos.getSignLon());
      devStatus.lonDeg.set(devStatus.geoPos.getDegLon());
      devStatus.lonMin.set(devStatus.geoPos.getMinLon());
      devStatus.lonSec.set(devStatus.geoPos.getSecLon());
      devStatus.height.set(gps.getAltitude());
      devStatus.latSign.set(devStatus.geoPos.getSignLat());
      devStatus.latDeg.set(devStatus.geoPos.getDegLat());
      devStatus.latMin.set(devStatus.geoPos.getMinLat());
      devStatus.latSec.set(devStatus.geoPos.getSecLat());
    }
    if (devPars.srcPosition.get() != static_cast<uint32_t>(enPositionMode::FIX))
      devStatus.satCount.set(gps.getSatCount());
    devStatus.gpsStatus.set(gps.getStatus());

    devStatus.mainLoop.set(loopCount);
    devStatus.peakVal.set(audio.getLastPeakVal() * 100);
    loopCount = 0;
    int sec = second();
    if(sec == 1)
    {
      devStatus.voltage.set(readSupplyVoltage());
      devStatus.chargeLevel = cBattery::getChargeCondition(devStatus.voltage.get());
      devStatus.battSymbol.set(cBattery::getBatterySymbol(devStatus.chargeLevel.get()));
    }
    else if(((minute() % 15) == 0) && (sec == 10))
      command.addToQueue(enCmd::MEAS_TEMPERATURE, nullptr);
    //  if(hasDisplay())
    //    /*cParGraph* g =*/ getLiveFft();
    devStatus.time.set();
    devStatus.date.set();
  }

  if(tick15Min.check())
    command.addToQueue(enCmd::UPDATE_INFOS, nullptr);
}
