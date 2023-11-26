#include <Arduino.h>

#include "cStatus.h"
#include "config.h"

MEMF const stStatusItem seqIDLE[] =
{
  {false, -1},
  {false, -1}
};

MEMF const stStatusItem seqON[] =
{
  {true, 3000},
  {false, 100},
  {false, -1}
}; 

MEMF const stStatusItem seqON100[] =
{
  {true,  100},
  {false, 200},
  {true,  100},
  {false, 200},
  {true,  100},
  {false, 200},
  {true,  100},
  {false, 4000},
  {false, 0}
}; 

MEMF const stStatusItem seqON75[] =
{
  {true,  100},
  {false, 200},
  {true,  100},
  {false, 200},
  {true,  100},
  {false, 4000},
  {false, 0}
}; 

MEMF const stStatusItem seqON50[] =
{
  {true,  100},
  {false, 200},
  {true,  100},
  {false, 4000},
  {false, 0}
}; 

MEMF const stStatusItem seqON25[] =
{
  {true,  100},
  {false, 4000},
  {false, 0}
}; 

void cStatus::show()
{
  if(_statusLed1 == enStatLed1::IDLE)
    return;

  uint32_t t = millis();
  if( t > _nextEndTime)
  {
    _psubStateLed1++;
    if(!_psubStateLed1->on && _psubStateLed1->ms == 0)
      _psubStateLed1 = _psubStateStart1;
    else if (_psubStateLed1->ms < 0)
      setStateLed1(enStatLed1::IDLE);
    setNewSubState();
  }
}

void cStatus::setStateLed1(enStatLed1 state)
{
  if(state == _statusLed1)
    return;
  switch (state)
  {
    case enStatLed1::IDLE:
      _psubStateStart1 = &seqIDLE[0];
      break;
    case enStatLed1::ON:
      _psubStateStart1 = &seqON[0];
      break;
    case enStatLed1::ON_100:
      _psubStateStart1 = &seqON100[0];
      break;
    case enStatLed1::ON_75:
      _psubStateStart1 = &seqON75[0];
      break;
    case enStatLed1::ON_50:
      _psubStateStart1 = &seqON50[0];
      break;
    case enStatLed1::ON_25:
      _psubStateStart1 = &seqON25[0];
      break;
  }
  _psubStateLed1 = _psubStateStart1;
  setNewSubState();
}

void cStatus::setNewSubState()
{
  _nextEndTime = _psubStateLed1->on +  millis();
  uint8_t led = _psubStateLed1->on ? 1 : 0;
  digWrite(SPIN_LED_DISP, led);
}