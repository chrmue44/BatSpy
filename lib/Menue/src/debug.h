/*************************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * Makros for debug outputs on the serial port
 * ***********************************************************/
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
    #define DPRINT4(x)       Serial.print (x);
    #define DPRINTLN4(x)     Serial.println (x);
    #define DPRINTF4(x, ...) Serial.printf(x, __VA_ARGS__);
  #else
    #define DPRINTLN4(x)
    #define DPRINTF4(x, ...)
  #endif
#elif defined SIMU_DISPLAY
#include <cstdio>
#include "Arduino.h"

#define DPRINTLN1(x) {char b[256]; snprintf(b, sizeof(b), x); Serial.log(b,1);}
#define DPRINTLN2(x) {char b[256]; snprintf(b, sizeof(b), x); Serial.log(b,2);}
#define DPRINTLN3(x) {char b[256]; snprintf(b, sizeof(b), x); Serial.log(b,3);}
#define DPRINTLN4(x) {char b[256]; snprintf(b, sizeof(b), x); Serial.log(b,4);}
#define DPRINT4(x)   {char b[256]; snprintf(b, sizeof(b),"%c", x); Serial.log(b,4);}
#define DPRINT1(x) { char b[256]; snprintf(b, sizeof(b), "%c", x); Serial.log(b,1);}
#define DPRINTF1(x, ...) { char b[256]; snprintf(b, sizeof(b), (x), __VA_ARGS__); Serial.log(b,1);}
#define DPRINTF2(x, ...) { char b[256]; snprintf(b, sizeof(b), (x), __VA_ARGS__); Serial.log(b,2);}
#define DPRINTF3(x, ...) { char b[256]; snprintf(b, sizeof(b), (x), __VA_ARGS__); Serial.log(b,3);}
#define DPRINTF4(x, ...) { char b[256]; snprintf(b, sizeof(b), (x), __VA_ARGS__); Serial.log(b,4);}
#else
  #define DPRINTLN1(x)
  #define DPRINTF1(x, ...)
  #define DPRINTLN2(x)
  #define DPRINTF2(x, ...)
  #define DPRINTLN3(x)
  #define DPRINTF3(x, ...)
  #define DPRINTLN4(x)
  #define DPRINT4(x)
  #define DPRINTF4(x, ...)
#endif


#endif  //#ifndef _DEBUG_H
