/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * A dead simple timer class to measure times
 * ***********************************************************/
#ifndef C_TIMER_H
#define C_TIMER_H

#include <cstdint>
#include "Arduino.h"


class cTimer
{
 public:
  cTimer() :
  m_startTime(0),
  m_alarmTime(0)
  {}
  
  void start()
  {
    m_startTime = millis();
    m_running = true;
  }

  void stop()
  {
    m_running = false;
  }

  bool isRunning() {return m_running;}

  /// @brief set alarm in <alarmtime_ms> milli seconds
  /// @param s time when alarm is signaled [ms]
  void setAlarm_ms(uint32_t alarmtime)
  {
    m_running = true;
    m_startTime = millis();
    m_alarmTime = alarmtime;
  }

  /// @brief set alarm in <alarmtime> seconds
  /// @param s time when alarm is signaled [s]
  void setAlarm(float alarm)
  {
    setAlarm_ms(alarm * 1000);
  }
  
  bool isAlarm()
  {
    return m_running & ( runTimeMs() > m_alarmTime);
  }

  uint32_t runTimeMs()
  {
    uint32_t actTime = millis();
    uint32_t retVal;
    if(actTime >= m_startTime)
      retVal = actTime - m_startTime;
    else
    {
      retVal = 0xFFFFFFFF - m_startTime;
      retVal += actTime;
    }
    return retVal;
  }

  float runTime()
  {
    return (float)runTimeMs()/1000.0f;
  }
 
 private:
   uint32_t m_startTime;
   uint32_t m_alarmTime;
   bool m_running = false;
};

#endif //#ifndef C_TIMER_H
