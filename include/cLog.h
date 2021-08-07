#ifndef _CLOG_H_
#define _CLOG_H_

class cLog
{
 public:
  static void log(const char* msg);
  static void logf(const char* fmt, ...);

 protected:
  cLog();
  static cLog* inst();

 private:
  void timeStamp();
  static cLog* m_inst;
  char m_fileName[24];
  char m_timeStamp[20];

};
#endif //#ifndef _CLOG_H_
