/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * Access to the real time clock
 * ***********************************************************/
#ifndef _CRTC_H_
#define _CRTC_H_
#include "TimeLib.h"
#include "types.h"
class cRtc 
{
 public:
  cRtc();
  void setTime(int y, int mo, int d, int h, int m, int s);
  int getTime (int& y, int& mo, int& d, int& h, int& m, int& s);
  time_t getTime();
  void checkAndSetTime(int y, int mo, int d, int h, int m, int s, enDlSaving ds);

 private:
 time_t maketime(int y, int mo, int d, int h, int m, int s);

   /// <summary>
   /// get daylight saving offset
   /// </summary>
   time_t getDaylightSaving(int y, int m, int d, enDlSaving ds);
   /// <summary>
   /// calculate day of week
   /// y: > 1752
   /// m: 1..12
   /// d: 1..31
   /// returns 0..6 (Sun .. Sat
   /// </summary>
   int dow(int y, int m, int d);
   /// <summary>
   /// checks wether current time needs dst adjustment or not 
   /// </summary>
   bool isDst(int y, int m, int d);


};
#endif //#ifndef _CRTC_H_
