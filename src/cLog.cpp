/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

//#define DEBUG_LEVEL 1
#include "debug.h"
#include "cLog.h"
#include "config.h"

#include <Arduino.h>
#include <TimeLib.h>
#include <stdarg.h>

MEMF cLog::cLog(const char* name)
:m_create(false)
{
  strncpy(m_name, name, sizeof(m_name));
}

void MEMF cLog::timeStamp()
{
    snprintf(m_timeStamp, sizeof(m_timeStamp),"%02i-%02i-%02i %02i:%02i:%02i",
             year() % 100, month(), day(), hour(), minute(), second());
}

void cLog::create()
{
  if(!m_create)
  {
    cSdCard& sd = cSdCard::inst();
    enSdRes ret = sd.chdir("/log");
    if(ret != enSdRes::OK) 
      ret = sd.mkDir("/log");
    snprintf(m_fileName, sizeof(m_fileName),"/log/%s_%02i%02i%02i-%02i%02i.log", 
             m_name, year()%100, month(),day(),hour(), minute());
    DPRINTF1("created file %s\n", m_fileName);
    m_create = true;
  }
}

void MEMF cLog::log(const char* msg, bool keepOpen)
{
  DPRINT1("cLog::log\n");
  create();
  timeStamp();
  cSdCard& sd = cSdCard::inst();
  if(!m_open)
  {
    sd.openFile(m_fileName, m_fd, enMode::APPEND);
    m_open = true;
  }
  char buf[256];
  if(msg[strlen(msg) - 1] == '\n')
    snprintf(buf, sizeof(buf), "%s %s", m_timeStamp, msg);
  else
    snprintf(buf, sizeof(buf), "%s %s\n", m_timeStamp, msg);
  
  size_t written;
  sd.writeFile(m_fd, buf, written, strlen(buf));
  if(!keepOpen)
  {
    sd.closeFile(m_fd);
    m_open = false;
  }
}

void MEMF cLog::logf(const char* fmt, ...)
{
  DPRINT1("cLog::logf\n");
  char buf[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  log(buf);
}

void MEMF cLog::close()
{
  if(m_open)
  {
    cSdCard::inst().closeFile(m_fd);
    m_open = false;
  }
}