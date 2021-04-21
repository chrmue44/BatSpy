#ifndef _DEBUG_H
#define _DEBUG_H

//#include "config.h"

#if defined DEBUG_LEVEL && !defined SIMU_DISPLAY
  #if DEBUG_LEVEL <= 1
    #define DPRINTLN1(x)     Serial.println (x);
    #define DPRINTF1(x, ...) Serial.printf(x, __VA_ARGS__);
  #else
    #define DPRINTLN1(x)
    #define DPRINTF1(x, ...)
  #endif
  #if DEBUG_LEVEL <= 2
    #define DPRINTLN2(x)     Serial.println (x);
    #define DPRINTF2(x, ...) Serial.printf(x, __VA_ARGS__);
  #else
    #define DPRINTLN2(x)
    #define DPRINTF2(x, ...)
  #endif
  #if DEBUG_LEVEL <= 3
    #define DPRINTLN2(x)     Serial.println (x);
    #define DPRINTF3(x, ...) Serial.printf(x, __VA_ARGS__);
  #else
    #define DPRINTLN3(x)
    #define DPRINTF3(x, ...)
  #endif
  #if DEBUG_LEVEL <= 4
    #define DPRINTLN4(x)     Serial.println (x);
    #define DPRINTF4(x, ...) Serial.printf(x, __VA_ARGS__);
  #else
    #define DPRINTLN4(x)
    #define DPRINTF4(x, ...)
  #endif
#elif defined SIMU_DISPLAY
#include "mainwindow.h"
#include <cstdio>
extern MainWindow* pw;

#define DPRINTLN1(x) {char b[128]; snprintf(b, sizeof(b), x); pw->log(b,1);}
#define DPRINTF1(x, ...) { char b[128]; snprintf(b, sizeof(b), (x), __VA_ARGS__); pw->log(b,1);}
#define DPRINTF2(x, ...) { char b[128]; snprintf(b, sizeof(b), (x), __VA_ARGS__); pw->log(b,2);}
#define DPRINTF3(x, ...) { char b[128]; snprintf(b, sizeof(b), (x), __VA_ARGS__); pw->log(b,3);}
#define DPRINTF4(x, ...) { char b[128]; snprintf(b, sizeof(b), (x), __VA_ARGS__); pw->log(b,4);}
#else
  #define DPRINTLN1(x)
  #define DPRINTF1(x, ...)
  #define DPRINTLN2(x)
  #define DPRINTF2(x, ...)
  #define DPRINTLN3(x)
  #define DPRINTF3(x, ...)
  #define DPRINTLN4(x)
  #define DPRINTF4(x, ...)
#endif


#endif  //#ifndef _DEBUG_H
