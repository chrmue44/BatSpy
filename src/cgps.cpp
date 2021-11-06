/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cgps.h"
#include "config.h"

void MEMF cGps::init()
{
  SERIAL_GPS.begin(4800);
  m_valid = false;
}

void MEMF cGps::operate()
{
  while (SERIAL_GPS.available())
  {
    int c = SERIAL_GPS.read();
    if (m_gps.encode(c))
    {
      float lat, lon;
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
      }
      m_gps.satellites
    }
  }
}