
// Please read Bounce.h for information about the liscence and authors

#include <Arduino.h>
#include "Bounce.h"


Bounce::Bounce(uint8_t pin,unsigned long interval_millis, uint32_t start_repeat_millis, uint32_t repeat_rate_millis)
:
m_pin(pin),
m_interval_millis(interval_millis),
m_start_repeat_millis(start_repeat_millis),
m_repeat_rate_millis(repeat_rate_millis),
m_state(enBounceState::RELEASED)
{
  m_previous_millis = millis();
  m_pinState = digitalRead(pin);
}


int Bounce::update()
{
  if ( debounce() )
  {
    if(!m_pinState)
	  m_state = enBounceState::PRESSED;
	else
	  m_state = enBounceState::RELEASED;
    return m_pinStateChanged = 1;
  }

  uint32_t actTime = millis();
  switch(m_state)
  {
	  case enBounceState::RELEASED:
	    break;
	  
    case enBounceState::PRESSED:
	    if(timeDiff(m_previous_millis, actTime) > m_start_repeat_millis)
	      m_state = enBounceState::REPEAT;
	    break;

	  case enBounceState::REPEAT:
        if (timeDiff(m_previous_millis, actTime) >= m_repeat_rate_millis)
      {
        m_previous_millis = actTime;
	      return m_pinStateChanged = 1;
	    }
	    break;
  }
  return m_pinStateChanged = 0;
}

// Protected: debounces the pin
int Bounce::debounce()
{
  uint8_t newPinState = digitalRead(m_pin);
  if (m_pinState != newPinState )
  {
	uint32_t actTime = millis();
    if (timeDiff(m_previous_millis, actTime) >= m_interval_millis)
    {
      m_previous_millis = actTime;
  	  m_pinState = newPinState;
      return 1;
	  }
  }

  return 0;
}

uint32_t Bounce::timeDiff(uint32_t start, uint32_t end)
{
  uint32_t retVal;
  if(end >= start)
    retVal = end - start;
  else
  {
    retVal = 0xFFFFFFFF - start;
    retVal += end;
  }
  return retVal;
}
