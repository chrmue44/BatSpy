/*********************************************************
 *         project: ProcessMon                           *
 *                                                       *
 *              (C) 2010 Christian Mueller               *
 *                       Odenwaldstrasse 134g            *
 *                       D-64372 Ober-Ramstadt           *
 *                                                       *
 *********************************************************/
#include "cltimeutils.h"
#ifdef LINUX
#include <unistd.h>
#endif

#ifdef WIN32
double clTimeUtils::m_Frequency = 0;
#endif

#include <math.h>

void clTimeUtils::sleep_ms(uint32_t Time)
{
  delayMicroseconds(Time * 1000);
}

void clTimeUtils::sleep_s(uint32_t Time)
{
  delayMicroseconds(Time * 1000000);
}

int32_t clTimeUtils::getSysTime(double& Time)
{
  int32_t err;
  struct timespec iTime; 

  err = getSysTime( &iTime);
  if(err == 0)
  {
    Time = iTime.tv_sec * 1000. + iTime.tv_nsec / 1000000.;
  }
  return err;
}


tTimeStamp clTimeUtils::calcDoubleToTimeStamp(double Seconds)
{
  uint64_t SecHigh;
  double FracLow;

  if (Seconds < 0)
    return 0;

  // Schneide Nachkommastellen ab
  SecHigh = (uint32_t)Seconds;  

  // Berechne 2^32-Ticks aus Nachkommastellen
  //  = 2^32 * Fraction(Seconds)

  FracLow = (double)(0x0100000000ULL) * (double)((double)Seconds - (uint32_t)Seconds);

  return ((SecHigh << 32) + (uint32_t)FracLow);

} // calcDoubleToTimeStamp


