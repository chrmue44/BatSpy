#include <Arduino.h>
#include <cSdCard.h>

#include "cStatus.h"
#include "config.h"
#include "globals.h"


void cStatus::nextState()
{
  switch (_ledMode)
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
  _state = enShowState::INIT;
}

void cStatus::show()
{
  switch(_state)
  {
    case enShowState::INIT:
    default:
      updateStatus();
      _blinkCnt1 = 0;
      _blinkCnt2 = 0;
      _state = enShowState::BLINK_MODE;
      if(_ledMode != enLedMode::DISP_IDLE)
      {
        _led1On = true;
        _blinkCnt1++;
        _timer.setAlarm_ms(LED_ON_TIME);
      }
      break;
      
    case enShowState::WAIT:
      if(_timer.isAlarm())
      {
        _state = enShowState::INIT;
      }
      break;

    case enShowState::BLINK_MODE:
      if(_timer.isAlarm())
      {
      if(_led1On)
      {
        _led1On = false;
        _timer.setAlarm_ms(LED_OFF_TIME);            
      }
      else
      {
        if(_blinkCnt1 < _maxBlinkCnt1)
        {
          _blinkCnt1++;
          _led1On = true;
          _timer.setAlarm_ms(LED_ON_TIME);            
        }
        else if((_maxBlinkCnt2 > 0) && (_statusLed2 != enStatLed::LED_OFF))
        {
          _led2On = true;
          _blinkCnt2++;
          _state = enShowState::BLINK_STATE;
          _timer.setAlarm_ms(LED_ON_TIME);            
        }
        else
        {
          _timer.setAlarm_ms(REPEAT_TIME);
          _state = enShowState::WAIT;
        }
      }
      }
      break;

    case enShowState::BLINK_STATE:
      if(_timer.isAlarm())
      {
        if(_led2On)
        {
          _led2On = false;
          _timer.setAlarm_ms(LED_OFF_TIME);            
        }
        else
        {
          if(_statusLed2 == enStatLed::LED_OFF)
            _led2On = false;
          else if (_statusLed2 == enStatLed::LED_ON)
            _led2On = true;
          else if(_blinkCnt2 < _maxBlinkCnt2)
          {
            _blinkCnt2++;
            _led2On = true;
          }
          if((_statusLed2 != enStatLed::LED_ON) && _led2On)
            _timer.setAlarm_ms(LED_ON_TIME);
          else
          {
            _timer.setAlarm_ms(REPEAT_TIME);
            _state = enShowState::WAIT;
          }
        }
      }
      break;  
  }
  if(_oldLed1On != _led1On)
  {
    digWrite(SPIN_LED_DISP, _led1On);
    _oldLed1On = _led1On;
  }
  if(_oldLed2On != _led2On)
  {
    digWrite(SPIN_LED_2, _led2On);
    _oldLed2On = _led2On;
  }
  return;
}

void cStatus::updateStatus()
{
  switch(_ledMode)
  {
    default:
    case enLedMode::DISP_IDLE:
      setStateLed2(enStatLed::LED_OFF);
      break;
    
    case enLedMode::DISP_REC:
      setStateLed2(enStatLed::ON_75);  //@@@ TODO
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
      setStateLed2(enStatLed::ON_50);  //@@@ TODO
      break; 
  }
}

void cStatus::setLedMode(enLedMode mode)
{
  _ledMode = mode;
  switch(mode)
  {
    case enLedMode::DISP_IDLE:
      _maxBlinkCnt1 = 0;
      break;
    case enLedMode::DISP_BATT:
      _maxBlinkCnt1 = 1;
      break;
    case enLedMode::DISP_SD:
      _maxBlinkCnt1 = 2;
      break;
    case enLedMode::DISP_REC:
      _maxBlinkCnt1 = 3;
      break;
    case enLedMode::DISP_GPS:
      _maxBlinkCnt1 = 4;
      break;
  }
}

void cStatus::setStateLed2(enStatLed state)
{
  if(state == _statusLed2)
    return;
  _statusLed2 = state;
  switch (state)
  {
    case enStatLed::LED_OFF:
      _maxBlinkCnt2 = 0;
      if(_led2On)
      {
        digWrite(SPIN_LED_DISP, 0);
        _led2On = false;
        _oldLed2On = false;
      }
      break;

    case enStatLed::LED_ON:
      if(!_led2On)
      {
        digWrite(SPIN_LED_DISP, 1);
        _led2On = true;
        _oldLed2On = true;
      }
      break;

    case enStatLed::ON_100:
      _maxBlinkCnt2 = 4;
      break;
    
    case enStatLed::ON_75:
      _maxBlinkCnt2 = 3;
      break;
    
    case enStatLed::ON_50:
      _maxBlinkCnt2 = 2;
      break;
    
    case enStatLed::ON_25:
      _maxBlinkCnt2 = 1;
      break;
  }
}
