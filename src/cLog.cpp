/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cLog.h"
#include "cSdCard.h"
#include "config.h"

#include <Arduino.h>
#include <TimeLib.h>
#include <stdarg.h>

MEMF cLog::cLog(const char* name)
{
  cSdCard& sd = cSdCard::inst();
  enSdRes ret = sd.chdir("/log");
  if(ret != enSdRes::OK) 
    ret = sd.mkDir("/log");
  snprintf(m_fileName, sizeof(m_fileName),"/log/%s_%02i%02i%02i-%02i%02i.log", 
           name, year()%100, month(),day(),hour(), minute());  
}

void MEMF cLog::timeStamp()
{
    snprintf(m_timeStamp, sizeof(m_timeStamp),"%02i-%02i-%02i %02i:%02i:%02i",
             year() % 100, month(), day(), hour(), minute(), second());
}

void MEMF cLog::log(const char* msg)
{
  timeStamp();
  cSdCard& sd = cSdCard::inst();
  tFILE f;
  sd.openFile(m_fileName, f, enMode::WRITE);
  char buf[256];
  snprintf(buf, sizeof(buf), "%s %s\n", m_timeStamp, msg);
  size_t written;
  sd.writeFile(f, buf, written, strlen(buf));
  sd.closeFile(f);  
}

void MEMF cLog::logf(const char* fmt, ...)
{
  char buf[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  log(buf);
}
