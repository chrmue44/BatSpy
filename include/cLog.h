/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The cLog class provides functionality to create and write
 * to log files
 * ***********************************************************/
#ifndef _CLOG_H_
#define _CLOG_H_

#include "cSdCard.h"

class cLog
{
 public:
  cLog(const char* name);
  void log(const char* msg, bool keepOpen = false);
  void logf(const char* fmt, ...);
  void close();

 protected:
  cLog();

 private:
  void timeStamp();
  void create();

  bool m_create;
  bool m_open = false;
  tFILE m_fd;

  char m_fileName[40];
  char m_timeStamp[20];
  char m_name[8];
};
#endif //#ifndef _CLOG_H_
