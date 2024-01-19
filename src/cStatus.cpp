#include <Arduino.h>
#include <cSdCard.h>
//#define DEBUG_LEVEL   4
#include "debug.h"
#include "cStatus.h"
#include "config.h"
#include "globals.h"


void cStatus::nextState()
{
  switch (m_ledMode)
  {
    case enLedMode::DISP_IDLE:
      setLedMode(enLedMode::DISP_BATT);
      break;
    case enLedMode::DISP_BATT:
      setLedMode(enLedMode::DISP_SD);
      break;
    case enLedMode::DISP_SD:   
      setLedMode(enLedMode::DISP_REC);
      break;
    default:
    case enLedMode::DISP_REC:  
      setLedMode(enLedMode::DISP_GPS);
      break;  
    case enLedMode::DISP_GPS:
      setLedMode(enLedMode::DISP_IDLE);
      break;
  }
  m_state = enShowState::INIT;
}

void cStatus::show()
{
  switch(m_state)
  {
    case enShowState::INIT:
    default:
      updateStatus();
      m_blinkCnt1 = 0;
      m_blinkCnt2 = 0;
      m_state = enShowState::BLINK_MODE;
      DPRINTLN4("cStatus  showState: BLINK_MODE");
      if(m_ledMode != enLedMode::DISP_IDLE)
      {
        m_led1On = true;
        m_blinkCnt1++;
        m_timer.setAlarm_ms(LED_ON_TIME);
      }
      break;
      
    case enShowState::WAIT:
      if(m_timer.isAlarm())
      {
        m_state = enShowState::INIT;
        DPRINTLN4("cStatus  showState: INIT");
      }
      break;

    case enShowState::BLINK_MODE:
      if(m_timer.isAlarm())
      {
      if(m_led1On)
      {
        m_led1On = false;
        m_timer.setAlarm_ms(LED_OFF_TIME);            
      }
      else
      {
        if(m_blinkCnt1 < m_maxBlinkCnt1)
        {
          m_blinkCnt1++;
          m_led1On = true;
          m_timer.setAlarm_ms(LED_ON_TIME);            
        }
        else if((m_maxBlinkCnt2 > 0) && (m_statusLed2 != enStatLed::LED_OFF))
        {
          m_led2On = true;
          m_blinkCnt2++;
          m_state = enShowState::BLINK_STATE;
          DPRINTLN4("cStatus  showState: BLINK_STATE");
          m_timer.setAlarm_ms(LED_ON_TIME);            
        }
        else
        {
          m_timer.setAlarm_ms(REPEAT_TIME);
          m_state = enShowState::WAIT;
          DPRINTLN4("cStatus showState: WAIT");
        }
      }
      }
      break;

    case enShowState::BLINK_STATE:
      if(m_timer.isAlarm())
      {
        if(m_statusLed2 == enStatLed::LED_OFF)
          m_led2On = false;
        else if (m_statusLed2 == enStatLed::LED_ON)
          m_led2On = true;
        else if(m_led2On)
        {
          m_led2On = false;
          m_timer.setAlarm_ms(LED_OFF_TIME);            
        }
        else
        {
          if(m_blinkCnt2 < m_maxBlinkCnt2)
          {
            m_blinkCnt2++;
            m_led2On = true;
          }
          if((m_statusLed2 != enStatLed::LED_ON) && m_led2On)
            m_timer.setAlarm_ms(LED_ON_TIME);
          else
          {
            m_timer.setAlarm_ms(REPEAT_TIME);
            m_state = enShowState::WAIT;
            DPRINTLN4("cStatus  showState: WAIT");
          }
        }
      }
      break;  
  }
  if(m_oldLed1On != m_led1On)
  {
    digWrite(SPIN_LED_DISP, m_led1On);
    m_oldLed1On = m_led1On;
  }
  if(m_oldLed2On != m_led2On)
  {
    digWrite(SPIN_LED_2, m_led2On);
    m_oldLed2On = m_led2On;
  }
  return;
}

void cStatus::execFunction()
{
  switch (m_ledMode)
  {
  case enLedMode::DISP_BATT:
  case enLedMode::DISP_IDLE:
  case enLedMode::DISP_SD:
  default:
    break;

  case enLedMode::DISP_GPS:
    if(gps.getStatus() == enGpsStatus::GPS_STATUS_OFF)
      gps.setMode(enGpsMode::GPS_ON);
    else
      gps.setMode(enGpsMode::GPS_OFF);
    break;

  case enLedMode::DISP_REC:
    if(m_recRunning)
      audio.stopRecording();
    else
      audio.startRecording();
    break;
  }
  m_state = enShowState::INIT;
}

void cStatus::setRecRunning(bool on)
{
  m_recRunning = on;
  updateStatus(); 
}

void cStatus::updateStatus()
{
  switch(m_ledMode)
  {
    default:
    case enLedMode::DISP_IDLE:
      setStateLed2(enStatLed::LED_OFF);
      break;
    
    case enLedMode::DISP_REC:
      if(m_recRunning)
        setStateLed2(enStatLed::LED_ON);
      else
        setStateLed2(enStatLed::LED_OFF);
      break;
    
    case enLedMode::DISP_GPS:
      {
        enGpsStatus status = gps.getStatus();
        switch(status)
        {
          case enGpsStatus::GPS_STATUS_OFF:
            setStateLed2(enStatLed::ON_25);
            break;
          case enGpsStatus::GPS_SEARCHING:
            setStateLed2(enStatLed::ON_50);
            break;
          case enGpsStatus::GPS_FIXED:
            setStateLed2(enStatLed::ON_75);
            break;
          case enGpsStatus::GPS_FIXED_OFF:
            setStateLed2(enStatLed::ON_100);
            break;
        }
      }
      break;

    case enLedMode::DISP_SD:
      {
        size_t total, free;
        enSdRes res = cSdCard::inst().getFreeMem(free, total);
        if(res == enSdRes::OK)
        {
          float ratio = (float)free/(float)total;
          if(ratio > 0.75)
            setStateLed2(enStatLed::ON_100);
          else if (ratio > 0.5)
            setStateLed2(enStatLed::ON_75);
          else if (ratio > 0.25)
            setStateLed2(enStatLed::ON_50);
          else
            setStateLed2(enStatLed::ON_25);
        }
      }
      break;

    case enLedMode::DISP_BATT:
      {
        float level = devStatus.chargeLevel.get(); 
          if(level > 80.0)
            setStateLed2(enStatLed::ON_100);
          else if (level > 60.0)
            setStateLed2(enStatLed::ON_75);
          else if (level > 30.0)
            setStateLed2(enStatLed::ON_50);
          else if (level > 10.0)
            setStateLed2(enStatLed::ON_25);
          else 
            setStateLed2(enStatLed::LED_OFF);
      }
      break; 
  }
}

void cStatus::setLedMode(enLedMode mode)
{
  m_ledMode = mode;
  switch(mode)
  {
    case enLedMode::DISP_IDLE:
      m_maxBlinkCnt1 = 0;
      break;
    case enLedMode::DISP_BATT:
      m_maxBlinkCnt1 = 1;
      break;
    case enLedMode::DISP_SD:
      m_maxBlinkCnt1 = 2;
      break;
    case enLedMode::DISP_REC:
      m_maxBlinkCnt1 = 3;
      break;
    case enLedMode::DISP_GPS:
      m_maxBlinkCnt1 = 4;
      break;
  }
}

void cStatus::setStateLed2(enStatLed state)
{
  if(state == m_statusLed2)
    return;
  m_statusLed2 = state;
  switch (state)
  {
    case enStatLed::LED_OFF:
      m_maxBlinkCnt2 = 0;
      if(m_led2On)
      {
        DPRINTLN4("cStatus LED2 OFF");
        digWrite(SPIN_LED_2, 0);
        m_led2On = false;
        m_oldLed2On = false;
      }
      break;

    case enStatLed::LED_ON:
      if(!m_led2On)
      {
        DPRINTLN4("cStatus LED2 ON");
        digWrite(SPIN_LED_2, 1);
        m_led2On = true;
        m_oldLed2On = true;
      }
      break;

    case enStatLed::ON_100:
      m_maxBlinkCnt2 = 4;
      break;
    
    case enStatLed::ON_75:
      m_maxBlinkCnt2 = 3;
      break;
    
    case enStatLed::ON_50:
      m_maxBlinkCnt2 = 2;
      break;
    
    case enStatLed::ON_25:
      m_maxBlinkCnt2 = 1;
      break;
  }
}
