#ifndef _CRTC_H_
#define _CRTC_H_
#ifndef SIMU_DISPLAY
#include <TimeLib.h>
#endif
class cRtc {
 public:
  cRtc();
  void setTime(int y, int mo, int d, int h, int m, int s);
  int getTime (int& y, int& mo, int& d, int& h, int& m, int& s);
  time_t getTime();
};
#endif //#ifndef _CRTC_H_
