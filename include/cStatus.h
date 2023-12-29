#ifndef _CSTATUS_H
#define _CSTATUS_H

/*
*  Display mode | LED 1 |  LED2
*  -------------+-------+---------------------------------------------
*  Batt         |  1x   |  100% = 4x, 75% = 3x, 50% = 2x, 25% = 1x
*  SD-Card      |  2x   |  100% = 4x, 75% = 3x, 50% = 2x, 25% = 1x
*  Rec          |  3x   |  active = 1x, inactive = 0x, recording = ON
*/

#include "stdint.h"
#include "cTimer.h"

#define LED_ON_TIME  70
#define LED_OFF_TIME 300
#define REPEAT_TIME  3000

enum enStatLed
{
  LED_OFF = 0,
  LED_ON,
  ON_100,
  ON_75,
  ON_50,
  ON_25  
};

enum enLedMode
{
  DISP_IDLE = 0,
  DISP_BATT = 1,
  DISP_SD = 2,
  DISP_REC = 3,
  DISP_GPS = 4
};

enum enShowState
{
  INIT = 0,
  WAIT = 1,
  BLINK_MODE = 2,
  BLINK_STATE = 3
};

class cStatus
{
  private:
   enStatLed m_statusLed2 = enStatLed::LED_OFF;
   enLedMode m_ledMode = enLedMode::DISP_IDLE;
   uint32_t m_lastT = 0;
   uint32_t m_maxBlinkCnt1 = 0;   
   uint32_t m_maxBlinkCnt2 = 0;   
   uint32_t m_blinkCnt1 = 0;   
   uint32_t m_blinkCnt2 = 0;
   enShowState m_state = enShowState::INIT; 
   bool m_led1On = false;
   bool m_oldLed1On = true;
   bool m_led2On = false;
   bool m_oldLed2On = true;
   cTimer m_timer;
   bool m_recRunning = false;

  public:
   cStatus() {}
   void show();
   void nextState();
   void setRecRunning(bool on);

  private:
   void setStateLed2(enStatLed state);
   void setLedMode(enLedMode mode);
   void updateStatus();
};

#endif //#ifndef _CSTATUS_H
