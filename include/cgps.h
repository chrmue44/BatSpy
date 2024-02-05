/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/
#ifndef CGPS_H
#define CGPS_H

#include <Arduino.h>
#include "TinyGPS.h"
#include "cgpx.h"
#include "cTimer.h"

#define MAX_ON_TIME_S  300 // max. ON time in AUTO_OFF mode

enum enGpsMode
{
  GPS_OFF = 0,
  GPS_ON = 1,
  GPS_AUTO_OFF_AFTER_FIX,
};

enum enGpsStatus
{
  GPS_STATUS_OFF = 0,
  GPS_SEARCHING = 1,
  GPS_FIXED = 2,
  GPS_FIXED_OFF = 3
};

class cGps
{
  public:
    void init();
    void setMode(enGpsMode mode);
    bool operate(bool testMode = false);
    int getLonDeg() { return static_cast<int>(m_lon); }
    enGpsStatus getStatus() { return m_status; }
    float getLat() { return m_lat; }
    float getLon() { return m_lon; }
    float getAltitude() { return m_altitude; }
    int getSatCount() { return m_satCount; }
    float getLonMin() { return m_lon - static_cast<int>(m_lon); }
    int getLatDeg() { return static_cast<int>(m_lat); }
    float getLatMin() { return m_lat - static_cast<int>(m_lat); }
    void openLog() { m_gpx.open();}
    void closeLog() { m_gpx.close();}
    bool isLogOpen() { return m_gpx.isOpen();}
    time_t getTime();
    void getTime(int& y, int& m, int& d, int& h, int& min, int& s);
    void test();

 private:
   TinyGPS m_gps;
   cGpx m_gpx;
   float m_lat;
   float m_lon;
   unsigned long m_age;
   float m_speed;
   float m_heading;
   float m_altitude;
   int m_satCount;
   bool m_valid;
   int m_year;
   byte m_byte;
   byte m_month;
   byte m_day;
   byte m_hour;
   byte m_minute;
   byte m_second;
   byte m_hundreds;
   char m_recLine[200];
   uint32_t m_recIdx;
   cTimer m_timer;
   bool m_power = false;
   enGpsMode m_mode = enGpsMode::GPS_OFF;
   enGpsStatus m_status =enGpsStatus::GPS_STATUS_OFF;
};

#endif //#ifndef CGPS_H
