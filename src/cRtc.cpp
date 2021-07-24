#include <Arduino.h>
#include "cRtc.h"


time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

cRtc::cRtc() {
  setSyncProvider(getTeensy3Time);
  ::setTime(Teensy3Clock.get());
}

void cRtc::setTime(int y, int mo, int d, int h, int m, int s) {
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


int cRtc::getTime(int& y, int& mo, int& d, int& h, int& m, int& s) {
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

time_t cRtc::getTime()
{
  return Teensy3Clock.get();
}
