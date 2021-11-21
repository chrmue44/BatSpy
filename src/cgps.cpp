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
  m_recIdx = 0;
}

void MEMF cGps::test()
{
  char esc[2];
  float lat,lon;
  do
  {
    operate(lat, lon, true);
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


void MEMF cGps::operate(float& lat, float& lon, bool testMode)
{
  while (SERIAL_GPS.available())
  {
    int c = SERIAL_GPS.read();
    if((c == '$') || (m_recIdx >= (sizeof(m_recLine) - 1))) 
    {
      m_recLine[m_recIdx] = 0;
      gpsLog.log(m_recLine, true);
      m_recIdx = 0;
    }
    m_recLine[m_recIdx++] = c;
    if(testMode)
      Serial.print((char)c);
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
