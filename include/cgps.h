/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include <Arduino.h>
#include "TinyGPS.h"
#include "cgpx.h"

class cGps
{
  public:
    void init();
    void operate(float& lat, float& lon);
    int getLonDeg() { return static_cast<int>(m_lon); }
    float getLonMin() { return m_lon - static_cast<int>(m_lon); }
    int getLatDeg() { return static_cast<int>(m_lat); }
    float getLatMin() { return m_lat - static_cast<int>(m_lat); }
    void openLog() { m_gpx.open();}
    void closeLog() { m_gpx.close();}
    bool isLogOpen() { return m_gpx.isOpen();}

 private:
   TinyGPS m_gps;
   cGpx m_gpx;
   float m_lat;
   float m_lon;
   unsigned long m_age;
   float m_speed;
   float m_heading;
   float m_altitude;
   bool m_valid;
   int m_year;
   byte m_byte;
   byte m_month;
   byte m_day;
   byte m_hour;
   byte m_minute;
   byte m_second;
};

extern cGps gps;
