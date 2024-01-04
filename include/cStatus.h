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

#define LED_ON_TIME  30
#define LED_OFF_TIME 350
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

/// @brief class to display several status functions with 2 LEDs
class cStatus
{
  private:
   enStatLed m_statusLed2 = enStatLed::LED_OFF;    // status od LED2
   enLedMode m_ledMode = enLedMode::DISP_IDLE;     // function or mode to display
   uint32_t m_lastT = 0;                           // last wait time
   uint32_t m_maxBlinkCnt1 = 0;                    // nr of blinks to display for LED1
   uint32_t m_maxBlinkCnt2 = 0;                    // nr of blinks to display for LED2
   uint32_t m_blinkCnt1 = 0;                       // number of blinks executed for LED1
   uint32_t m_blinkCnt2 = 0;                       // number of blinks executed for LED2
   enShowState m_state = enShowState::INIT;        // state of state machine to display 
   bool m_led1On = false;                          // current status LED1
   bool m_oldLed1On = true;                        // previous status LED1
   bool m_led2On = false;                          // current status LED2
   bool m_oldLed2On = true;                        // previous status LED1
   cTimer m_timer;
   bool m_recRunning = false;

  public:
   cStatus() {}
   /// @brief show currently selected function (runs the state machine)
   void show();

   /// @brief switch to next display mode
   void nextState();
   
   /// @brief set recording status ON or OFF
   /// @param on recording status
   void setRecRunning(bool on);
   
   /// @brief execute function associated to current display mode
   void execFunction();

  private:
   void setStateLed2(enStatLed state);
   void setLedMode(enLedMode mode);
   void updateStatus();
};

#endif //#ifndef _CSTATUS_H
