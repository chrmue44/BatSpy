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

void MEMF cGps::init()
{
  SERIAL_GPS.begin(9800, SERIAL_8N1);
  cSdCard::inst().chdir("/log");
  m_valid = false;
  m_pRec = m_recLine;
}

void MEMF cGps::operate(float& lat, float& lon)
{
  while (SERIAL_GPS.available())
  {
    int c = SERIAL_GPS.read();
    *m_pRec++ = c;
    if(c == '\n')
    {
      gpsLog.log(m_recLine);
      m_pRec = m_recLine;
    }
    DPRINT1((char)c);
    if (m_gps.encode(c))
    {
      int year;
      byte month, day, hour, minute, second, hundredths;
      m_gps.f_get_position(&lat, &lon, &m_age);
      m_gps.crack_datetime(&year, &month, &day, &hour,
                           &minute, &second, &hundredths, &m_age);
      if (m_age == TinyGPS::GPS_INVALID_AGE)
        m_valid = false;
      else if(m_age < 5000)
      {
        m_lat = lat;
        m_lon = lon;
        m_speed = m_gps.f_speed_kmph();
        m_heading = m_gps.f_course();
        m_altitude = m_gps.f_altitude();
        m_year = year;
        m_month = month;
        m_day = day;
        m_hour = hour;
        m_minute = minute;
        m_second = second;
        if(m_gpx.isOpen())
          m_gpx.log(lat, lon, m_altitude);
      }
    }
  }
}
