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
#include <TimeLib.h>
class cRtc {
 public:
  cRtc();
  void setTime(int y, int mo, int d, int h, int m, int s);
  int getTime (int& y, int& mo, int& d, int& h, int& m, int& s);
  time_t getTime();
};
#endif //#ifndef _CRTC_H_
