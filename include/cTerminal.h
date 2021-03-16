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
  tKey getKey() { tKey key = m_key; m_key = DEV_KEY_NOKEY; return key; }
  
 private:
  void execCmd();
  
  char m_recbuf[256];
  int m_recIdx = 0;
  int m_sendIdx = 0;
  tKey m_key = DEV_KEY_NOKEY;
};

#endif //#ifndef CTERMINAL_H
