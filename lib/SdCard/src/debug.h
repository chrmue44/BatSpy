#ifndef _DEBUG_H
#define _DEBUG_H

//#include "config.h"

#if defined DEBUG_LEVEL && !defined SIMU_DISPLAY
  #if DEBUG_LEVEL <= 1
    #define DPRINT1(x)     Serial.print (x)
    #define DPRINTLN1(x)     Serial.println (x)
    #define DPRINTF1(x, ...) Serial.printf(x, __VA_ARGS__)
  #else
    #define DPRINT1(x)
    #define DPRINTLN1(x)
    #define DPRINTF1(x, ...)
  #endif
  #if DEBUG_LEVEL <= 2
    #define DPRINT2(x)     Serial.print (x)
    #define DPRINTLN2(x)     Serial.println (x);
    #define DPRINTF2(x, ...) Serial.printf(x, __VA_ARGS__);
  #else
    #define DPRINT2(x)
    #define DPRINTLN2(x)
    #define DPRINTF2(x, ...)
  #endif
  #if DEBUG_LEVEL <= 3
    #define DPRINT3(x)     Serial.print (x)
    #define DPRINTLN2(x)     Serial.println (x);
    #define DPRINTF3(x, ...) Serial.printf(x, __VA_ARGS__);
  #else
    #define DPRINT3(x)
    #define DPRINTLN3(x)
    #define DPRINTF3(x, ...)
  #endif
  #if DEBUG_LEVEL <= 4
    #define DPRINT4(x)     Serial.print (x)
    #define DPRINTLN4(x)     Serial.println (x);
    #define DPRINTF4(x, ...) Serial.printf(x, __VA_ARGS__);
  #else
    #define DPRINT4(x)
    #define DPRINTLN4(x)
    #define DPRINTF4(x, ...)
  #endif
#else
  #define DPRINT1(x)
  #define DPRINTLN1(x)
  #define DPRINTF1(x, ...)
  #define DPRINT2(x)
  #define DPRINTLN2(x)
  #define DPRINTF2(x, ...)
  #define DPRINT3(x)
  #define DPRINTLN3(x)
  #define DPRINTF3(x, ...)
  #define DPRINT4(x)
  #define DPRINTLN4(x)
  #define DPRINTF4(x, ...)
#endif


#endif  //#ifndef _DEBUG_H
