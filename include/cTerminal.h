/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}.com             *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The terminal interface via the serial line to operate
 * BatSpy via terminal
 * ***********************************************************/
#ifndef CTERMINAL_H
#define CTERMINAL_H

#include <MenueSystem.h>

class cTerminal {
 public:
  cTerminal();
  void parseCmd();
  void showCommands();
  void parseControlCmd(const char* buf);
  void parseSetCmd(const char* buf);
  enKey getKey() { enKey key = m_key; m_key = NOKEY; return key; }
  
 private:
  void execCmd();
  
  char m_recbuf[256];
  int m_recIdx = 0;
  int m_sendIdx = 0;
  enKey m_key = NOKEY;
};

#endif //#ifndef CTERMINAL_H
