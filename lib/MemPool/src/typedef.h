#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_
/*********************************************************
 *         project: MemPool Library                      *
 *                                                       *
 *          Author: Christian Mueller                    *
 *                  Gutenbergstr 20                      *
 *                  D-64342 Seeheim-Jugenheim            *
 *                                                       *
 *********************************************************/

#include <Arduino.h>
#include <cstdint>
#include <iostream>

using namespace std;


typedef int32_t tTXT_HANDLE;

typedef uint64_t tPM_UUID;

#define MIN_DOUBLE -1.7976931348623158e+308
#define MAX_DOUBLE 1.7976931348623158e+308
#define MIN_FLOAT -3.4e+38
#define MAX_FLOAT 3.4e+38
#define MAX_ZERO_DOUBLE 1.0e-300
#define MIN_INT32  -0x7FFFFFFF
#define MAX_INT32  0x7FFFFFFF
#define MAX_UINT32  0xFFFFFFFF
#define MAX_REAL static_cast<REAL>(MAX_FLOAT)
#define MIN_REAL static_cast<REAL>(MIN_FLOAT)
#define EPS_REAL 1e-10

#define MAX_UINT64 static_cast<UINT64>(0xFFFFFFFFFFFFFFFFll)
#define MIN_UINT64 static_cast<UINT64>(0ll)
#define MAX_INT64 static_cast<INT64>(0x7FFFFFFFFFFFFFFFll)
#define MIN_INT64 static_cast<INT64>(0x8000000000000000ll)

// tTimeStamp: 
//  obere 32 Bit =  Zeit in Sekunden 
//  untere 32 Bit = Nachkomma der Sekunden in Aufloesung von 2^-32 Sekunden 
typedef uint64_t tTimeStamp;

typedef int32_t tTime_ms;//eine Zeitangabe in Milli-Sekunden 
typedef int32_t tTime_s;// eine Zeitangabe in Sekunden


enum tCM_BOOL
{
  CM_TRUE = 1,
  CM_FALSE = 0
};



#define CM_DOUBLE_PRECISION 14

#define CM_PI 3.141592653589793

enum tLogMode
{
  // Ausgabe auf der Konsole
  COUT       = 0,
  // Ausgabe in ein ASCII-File
  LOG_ASCII  = 1,
  // Ausgabe in ein Binaer-File (verwenden von LogView.exe)
  LOG_BINAER = 2

    
  // Ausgabe als ASCII-Stream (Datei lokal oder im Netz, die immer groesser wird)
//  ASCII_STREAM_APP   = 2,
  // Ausgabe als formatiertes PM-Log-File
//  PM_LOG            = 1,
  // Ausgabe als ASCII-Stream (Datei lokal oder im Netz, die immer groesser wird)
//  ASCII_STREAM_APP   = 2,
  // Ausgabe als ASCII-Stream (Datei lokal oder im Netz, die groessenbegrenzt ist)
//  ASCII_STREAM_WRAP  = 3,
  // Ausgabe als formatiertes PM-Log-File (Datei lokal oder im Netz, die immer groesser wird)
//  BINAER_STREAM_APP  = 4,
  // Ausgabe als formatiertes PM-Log-File (Datei lokal oder im Netz, die groessenbegrenzt ist)
//  BINAER_STREAM_WRAP = 5,
};

// Format, Daten in der Log-Dateien append oder wrap
enum tLogFormat
{
  // Ausgabe ans File-Ende anhaengen (Datei lokal oder im Netz, die immer groesser wird)
  LOG_APPEND = 0,
  // Ausgabe ans File-Ende anhaengen (Datei lokal oder im Netz, die immer groesser wird)
  LOG_WRAP   = 1

  // Ausgabe als Stream (Datei lokal oder im Netz, die immer groesser wird)
  // LOG_STREAM   = 0,
  // Ausgabe wrappen (Datei lokal oder im Netz, die groessenbegrenzt ist)
  // LOG_WRAP  = 1
};

#define LOGVIEWER_DBGSRC_OFFS 0x10000000
enum tDbgLogSrc
{
  // logging von Mutex-Locks und unlocks
  DBGSRC_MTXS        = 0x00000001,
  // Allokierung vom Memory-Pool
  DBGSRC_MEMFIXALLOC = 0x00000002,
  // Allokierung vom HEAP, weil Memeory-Pool erschoepft
  DBGSRC_MEMDYNALLOC = 0x00000004,
  // Allokierung des Speichers durch STL-Container
  DBGSRC_STLALLOC    = 0x00000008,
  // Webserver (HTTP Server)
  DBGSRC_HTTPSERVER  = 0x00000010,
  // Messagequeue des DSS_com
  DBGSRC_COMMSGHNDLR = 0x00000020,
  // Par log
  DBGSRC_PAR         = 0x00000040,
  // Daq log
  DBGSRC_DLL         = 0x00000080,
  // HAL log
  DBGSRC_HAL         = 0x00000100,
  // MSP log
  DBGSRC_MSP         = 0x00000200,
  // System log
  DBGSRC_SYS         = 0x00000400,
  // Logging zu Thread-Sarts und Ends
  DBGSRC_THREADS     = 0x00000800,
  // TEDS Log
  DBGSRC_TEDS        = 0x00001000,
  // Logging zu Timesync
  DBGSRC_TIMESYNC   =  0x00002000
};

#ifndef SIMU_DISPLAY
#define PM_ASSERT(condition, message){ \
  if(!(condition)) {\
  Serial.printf("assert line : %i, %s\n",  __LINE__  , message);;\
  /*TBD*/}\
}
#else
#define PM_ASSERT(condition, message){ \
  if(!(condition)) {\
  std::cout << "assert line :" <<  __LINE__  << message << std::endl;}\
}
#endif

#endif //#ifndef TYPEDEF_H
