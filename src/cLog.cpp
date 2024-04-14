/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include <Arduino.h>
#include <TimeLib.h>
#include <stdarg.h>

#include "cLog.h"
#include "config.h"
//#define DEBUG_LEVEL 1
#include "debug.h"



MEMF cLog::cLog(const char* name)
:m_create(false)
{
  strncpy(m_name, name, sizeof(m_name) - 1);
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
    DPRINTF1("created file name %s\n", m_fileName);
    enSdRes res = sd.openFile(m_fileName, m_fd, enMode::WRITE);
    if(res == enSdRes::OK)
    {
      DPRINTLN1("initialized log");
      size_t written;
      char buf[256];
      snprintf(buf, sizeof(buf), "log created\n");
      sd.writeFile(m_fd, buf, written, strlen(buf));
      sd.closeFile(m_fd);
    }
    else
      DPRINTLN1("failed to open file");
    m_create = true;
  }
  timeStamp();
  if(!m_open)
  {
    enSdRes res =  cSdCard::inst().openFile(m_fileName, m_fd, enMode::APPEND);
    DPRINTF1("open log file: %s, res %i\n", m_fileName, res);
    if(res == enSdRes::OK)
      m_open = true;
    else
      DPRINTLN1("failed to open file");
  }
}

void MEMF cLog::debugLog(int level, const char c, bool keepOpen)
{
  if(m_logLevel >= level)
  {
    create();
    if (m_open)
    {
      size_t written;
      cSdCard::inst().writeFile(m_fd, &c, written, 1);
      if (!keepOpen)
      {
        DPRINTLN1("logfile closed in log()");
        cSdCard::inst().closeFile(m_fd);
        m_open = false;
      }
    }
  }
}

void MEMF cLog::log(const char* msg, bool keepOpen)
{
  DPRINTF1("cLog::log: %s\n", msg);
  create();
  if (m_open)
  {
    char buf[256];
    if (msg[strlen(msg) - 1] == '\n')
      snprintf(buf, sizeof(buf), "%s %s", m_timeStamp, msg);
    else
      snprintf(buf, sizeof(buf), "%s %s\n", m_timeStamp, msg);

    size_t written;
    cSdCard::inst().writeFile(m_fd, buf, written, strlen(buf));
    if (!keepOpen)
    {
      DPRINTLN1("logfile closed in log()");
      cSdCard::inst().closeFile(m_fd);
      m_open = false;
    }
  }
}

void MEMF cLog::debugLog(int level, const char* msg) 
{
  if(level == 0)
    return;
  if (level >= m_logLevel)
    logf(msg);
}

void MEMF cLog::logf(const char* fmt, ...)
{
  DPRINTLN1("cLog::logf");
  char buf[256];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);
  log(buf, true);
}

void MEMF cLog::close()
{
  DPRINTF1("close log %i\n", m_open);
  if(m_open)
  {
    char buf[128];
    snprintf(buf, sizeof(buf), "close log\n");
    log(buf);
    cSdCard::inst().closeFile(m_fd);
    m_open = false;
  }
}