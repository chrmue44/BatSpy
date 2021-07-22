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
Metro tick(300);
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
  pinMode(PIN_TFT_LED, OUTPUT);
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
  size_t freeMem, totMem;
  audio.init();
  Serial.begin(9600);
  Serial.println("setting up bat detector");
//  waitForSerial();
  initTft();
  tft.println("setting up bat detector");
  delay(500);
  Txt::setResource(Texts);
  cSdCard::inst().mount();
  cSdCard::inst().getFreeMem(freeMem, totMem);
  Serial.printf("free memory on SD card: %u of %u [kB]\n", freeMem, totMem);
  delay(500);  
  menue.init();
  tft.setRotation(devPars.dispOrient.get() == 0 ? 3 : 1);
  menue.refreshAll();
  menue.printPars();
  audio.setup();
  wheels.setDirection(true);  
  setDispLight(255);
  devStatus.opMode.set(enOpMode::REC_AUTO);
  setVisibilityRecCount(&menue);
}


// *********************** main loop **************************

void loop() 
{
  static bool backLightOn = true;
  static uint32_t cnt;
  if (tick.check())
  {
    if(menue.getMainPanel() == panInfo)
    {
      cnt++;
      if(cnt > 3)
      {
        devStatus.cpuAudioAvg.set(AudioProcessorUsage());
        devStatus.cpuAudioMax.set(AudioProcessorUsageMax());
        devStatus.audioMem.set(AudioMemoryUsage());
        devStatus.peakVal.set(audio.getLastPeakVal() * 100);
        size_t freeSpace;  size_t totSpace;
        cSdCard::inst().getFreeMem(freeSpace, totSpace);
        int digits = analogRead(PIN_SUPPLY_VOLT);
        devStatus.digits.set((float)digits);
        float volt = (float)digits * devPars.voltFactor.get() + U_DIODE;
        DPRINTF2("digits: %i  voltage: %f  factor: %f\n", digits, volt,devPars. voltFactor.get());
        devStatus.voltage.set(volt);
        devStatus.freeSpace.set(freeSpace * 100.0 / totSpace);
        cnt = 0;
      }
    }
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
  }
  audio.operateRecorder();
}
