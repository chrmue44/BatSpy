/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/
//#define DEBUG_LEVEL  1
#include "debug.h"
#include "globals.h"
#include "config.h"
#include <cmath>

void MEMF cGps::init()
{
  if(m_power)
  {
    if(devPars.gpsBaudRate.get() == static_cast<uint32_t>(enGpsBaudRate::BD_9600))
      SERIAL_GPS.begin(9600, SERIAL_8N1);
    else
      SERIAL_GPS.begin(115200, SERIAL_8N1);
    cSdCard::inst().chdir("/log");
    m_valid = false;
    m_recIdx = 0;
  }
}

void MEMF cGps::setMode(enGpsMode mode)
{
  switch (mode)
  {
    default:
    case enGpsMode::GPS_OFF:
      digWrite(SPIN_PWR_GPS, 0);
      m_power = false;
      break;
    case enGpsMode::GPS_ON:
      if (m_mode == enGpsMode::GPS_OFF)
        m_status = enGpsStatus::GPS_SEARCHING;
      digWrite(SPIN_PWR_GPS, 1);
      m_power = true;
      gps.init();
      break;
    case enGpsMode::GPS_AUTO_OFF_AFTER_FIX:
      if (m_mode == enGpsMode::GPS_OFF)
        m_status = enGpsStatus::GPS_SEARCHING;
      digWrite(SPIN_PWR_GPS, 1);
      m_timer.setAlarm_ms(MAX_ON_TIME_S * 1000);
      m_power = true;
      gps.init();
      break;
    break;
  }
  m_mode = mode;
}

time_t MEMF cGps::getTime()
{
  if(m_valid)
  {
    tmElements_t tim;
    tim.Year = m_year;
    tim.Month = m_month;
    tim.Day = m_day;
    tim.Hour = m_hour;
    tim.Minute = m_minute;
    tim.Second = m_second;
    return makeTime(tim);
  }
  else
    return 0;
}

void MEMF cGps::getTime(int& y, int& m, int& d, int& h, int& min, int& s)
{
  y = m_year;
  m = m_month;
  d = m_day;
  h = m_hour;
  min = m_minute;
  s = m_second;
}


void MEMF cGps::test()
{
  char esc[2] = {0,0};

  do
  {
    operate(true);
    while(Serial.available() > 0)
    {
      char c = Serial.read();
      esc[1] = esc[0];
      esc[0] = c;
      if((esc[1] != 'q') || (esc[0] != '!'))
        SERIAL_GPS.write(c);
      else
       break;
    }
  } while ((esc[1] != 'q') || (esc[0] != '!'));
}


bool MEMF cGps::operate(bool testMode)
{
  if(m_mode == enGpsMode::GPS_OFF)
    m_status = enGpsStatus::GPS_STATUS_OFF;
  if(m_power)
  {
#ifdef SIMU_DISPLAY
    m_lat = 49 + (float)std::rand() / RAND_MAX;
    m_lon = 8 + (float)std::rand() / RAND_MAX;
    m_satCount = 3 + 3 * (float)std::rand() / RAND_MAX;
    m_status = enGpsStatus::GPS_FIXED;
    m_valid = true;
#else
    if(testMode && (SERIAL_GPS.available() == 0))
    {
      Serial.println("no GPS data");
      delay(1000);
    }
    while (SERIAL_GPS.available()) 
    {
      int c = SERIAL_GPS.read();
      if((c == '$') || (m_recIdx >= (sizeof(m_recLine) - 1))) 
      {
        m_recLine[m_recIdx] = 0;
        if(devPars.debugLevel.get() >= 1)
          gpsLog.log(m_recLine, true);
        m_recIdx = 0;
      }
      m_recLine[m_recIdx++] = c;
      if(testMode)
        Serial.print((char)c);
   //   DPRINT1((char)c);
      if (m_gps.encode(c))
      {     
        m_gps.f_get_position(&m_lat, &m_lon, &m_age);
        m_gps.crack_datetime(&m_year, &m_month, &m_day, &m_hour,
                           &m_minute, &m_second, &m_hundreds, &m_age);
        if (m_age == TinyGPS::GPS_INVALID_AGE)
          m_valid = false;
        else if(m_age < 5000)
        {
          m_speed = m_gps.f_speed_kmph();
          m_heading = m_gps.f_course();
          m_altitude = m_gps.f_altitude();
          if(!m_valid)
          {
			command.addToQueue(enCmd::CHECK_AND_SET_TIME, nullptr);
            if (m_mode == enGpsMode::GPS_AUTO_OFF_AFTER_FIX)
            {
              m_timer.stop();
              m_timer.setAlarm(MAX_ON_TIME_S);
              m_timer.start();
            }
          }
		      m_valid = true;
          if(m_gpx.isOpen())
            m_gpx.log(m_lat, m_lon, m_altitude);
          
          m_satCount = m_gps.satellites();
          if(devPars.debugLevel.get() >= 1)
            gpsLog.logf("new position  lat:%f lon:%f  sat:%i age:%i\n", m_lat, m_lon, (int)m_satCount, m_age);
        }
        else
          m_valid = false;

        if(m_valid)
        {
          if((m_mode == enGpsMode::GPS_AUTO_OFF_AFTER_FIX) && (m_timer.isAlarm()))
          {
            m_status = enGpsStatus::GPS_FIXED_OFF;
            m_power = false;
            digWrite(SPIN_PWR_GPS, 0);
          }
          else
            m_status = enGpsStatus::GPS_FIXED;
        }
        else
          m_status = enGpsStatus::GPS_SEARCHING;
        if(devPars.debugLevel.get() >= 1)
          gpsLog.logf("valid: %i\n", m_valid ? 1: 0);
      }
    }
#endif  //#ifdef SIMU_DISPLAY
  }
  return m_valid;
}
