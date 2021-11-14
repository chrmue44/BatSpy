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

class cLog
{
 public:
  cLog(const char* name);
  void log(const char* msg);
  void logf(const char* fmt, ...);

 protected:
  cLog();

 private:
  void timeStamp();
  static cLog* m_inst;
  char m_fileName[40];
  char m_timeStamp[20];

};
#endif //#ifndef _CLOG_H_
