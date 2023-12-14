/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The terminal interface via the serial line to operate
 * BatSpy via terminal
 * ***********************************************************/
#ifndef CTERMINAL_H
#define CTERMINAL_H

#include <MenueSystem.h>

#define MAX_ONLINE_PAUSE 180000

class cTerminal {
 public:
  cTerminal();
  /// @brief parse commands incoming from USB
  /// @return true if hardware was reconfigured
  bool parseCmdfromUSB();

  void showCommands();
  void parseControlCmd(const char* buf);
  void parseSetCmd(const char* buf);
  void parseSetStatusCmd(const char* buf);
  void parseGetCmd(const char* buf);
  void parseGetStatusCmd(const char* buf);
  enKey getKey() { enKey key = m_key; m_key = NOKEY; return key; }
  bool isOnline();

 private:
  bool execCmd(char* buf, size_t& bufIdx);
  bool parseRecParams(const char* buf, bool write, char* replyBuf = nullptr, size_t replyBufLen = 0);
  bool parseLocationParams(const char* buf, bool write, char* replyBuf = nullptr, size_t replyBufLen = 0);
  bool parseAutoRecParams(const char* buf, bool write, char* replyBuf = nullptr, size_t replyBufLen = 0);
  bool checkValI(int val, int min, int max) { return (min <= val) && (val <= max); }
  bool checkValF(int val, int min, int max) { return (min <= val) && (val <= max); }
  bool setValInt(const char* buf, int min, int max, cParNum& par);
  void getValInt(const char* buf, cParNum& par, char* reply, size_t replySize);
  bool setValEnum(const char* buf, int min, int max, cParEnum& par);
  void getValEnum(const char* buf, cParEnum& par, char* reply, size_t replySize);
  bool setValFloat(const char* buf, float min, float max, cParNum& par);
  void getValFloat(const char* buf, cParNum& par, char* reply, size_t replySize);
  void getPosition(char* buf, size_t bufSize);

  char m_recbufUSB[256];
  char m_recbufESP[256];
  size_t m_recIdxUSB = 0;
  size_t m_recIdxESP = 0;
  int m_sendIdx = 0;
  enKey m_key = NOKEY;
  char m_endChar = 0x04; // '\n'
  uint32_t m_onlineTime = 0; 
};


#endif //#ifndef CTERMINAL_H
