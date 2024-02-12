/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cRtc.h"
#include <Arduino.h>
//#include "TimeLib.h"
#include "globals.h"

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

cRtc::cRtc()
{
  setSyncProvider(getTeensy3Time);
  ::setTime(Teensy3Clock.get());
}

void cRtc::setTime(int y, int mo, int d, int h, int m, int s) 
{
  tmElements_t tm;
  if(y > 1970)
   y -= 1970;
  tm.Year = y;
  tm.Month = mo;
  tm.Day = d;
  tm.Hour = h;
  tm.Minute = m;
  tm.Second = s;
  time_t t = makeTime(tm);
  Teensy3Clock.set(t); // set the RTC
  ::setTime(t);
}

time_t MEMF cRtc::maketime(int y, int mo, int d, int h, int m, int s)
{
  tmElements_t tim;
  if(y > 1970)
    y -= 1970;
  tim.Year = y;
  tim.Month = mo;
  tim.Day = d;
  tim.Hour = h;
  tim.Minute = m;
  tim.Second = s;
  return ::makeTime(tim);
}

void MEMF cRtc::checkAndSetTime(int y, int mo, int d, int h, int m, int s, enDlSaving ds)
{
  time_t dst = getDaylightSaving(y, mo, d, ds);
  time_t gpsTime = dst + (long)(devPars.timeZone.get()) * 3600 + maketime(y, mo, d, h, m, s);
  sysLog.log("check and set time");
  if(abs(getTime() - gpsTime) > 20)
  {
    sysLog.logf("set time to %02i.%02i.%04i   %02i:%02i:%02i", d, mo, y, h, m, s);
    Teensy3Clock.set(gpsTime);
    ::setTime(gpsTime);
  } 
}

int MEMF cRtc::getTime(int& y, int& mo, int& d, int& h, int& m, int& s)
{
 // tmElements_t tm;
 // time_t t = Teensy3Clock.get();
 // breakTime(t, tm);  // break time_t into elements
  y = year();
  if(y < 1970)
    y += 1970;
  mo = month();
  d = day();
  h = hour();
  m = minute();
  s = second();
  return 0;
}


int cRtc::dow(int y, int m, int d)	/* 1 <= m <= 12,  y > 1752 (in the U.K.) */
{
  uint8_t t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };
  if (m < 3)
    y -= 1;
  return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

//https://stackoverflow.com/questions/5590429/calculating-daylight-saving-time-from-only-date
bool cRtc::isDst(int y, int m, int d)
{
  if ((m < 3) || (m > 10))  
    return false;
  if ((m > 3) && (m < 10))  
    return true;

  int previousSunday = d - dow(y, m, d);

  if (m == 3) 
    return previousSunday >= 25;
  if (m == 10) 
    return previousSunday < 25;

  return false; // this line never gonna happend
}

time_t cRtc::getDaylightSaving(int y, int m, int d, enDlSaving ds)
{
  time_t retVal = 0;
  switch (ds)
  {
    case enDlSaving::DLS_OFF:
      retVal = 0;
      break;
    case enDlSaving::DLS_ON:
      retVal = 3600;
      break;
    case enDlSaving::DLS_AUTO:
      retVal = isDst(y, m, d) ? 3600 : 0;
      break;
  }
  return retVal;
}

time_t cRtc::getTime()
{
  return Teensy3Clock.get();
}
