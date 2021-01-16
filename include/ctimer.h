#ifndef C_TIMER_H
#define C_TIMER_H

#include <cstdint>
#include "Arduino.h"

class cTimer
{
 public:
  cTimer() :
  m_startTime(0)
  {}
  
  void start() {
    m_startTime = millis();
  }

  uint32_t runTimeMs() {
    uint32_t actTime = millis();
    uint32_t retVal;
    if(actTime > m_startTime)
      retVal = actTime - m_startTime;
    else {
      retVal = 0xFFFFFFFF - m_startTime;
      retVal += actTime;     
    }
    return retVal;
  }

  float runTime() {
    return (float)runTimeMs()/1000.0;
  }
 
 private:
   uint32_t m_startTime;
};

#endif //#ifndef C_TIMER_H
