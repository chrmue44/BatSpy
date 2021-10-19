#ifndef CPRJELEKON_H
#define CPRJELEKON_H

//#include <string>
//#include <vector>
//#include <filesystem>
//namespace fs = std::experimental::filesystem;
#include "my_vector.h"
#include "cxmlhelper.h"
#include "types.h"

struct stRecord
{
  char wavFile[40];
  char name[40];
};

class cPrjoject
{
public:
  cPrjoject();
  void openPrjFile();
  void closePrjFile();
  void addFile();
  void writeInfoFile(float peakVal, size_t sampleCnt, enRecFmt recFmt);

  bool getIsOpen() { return m_isOpen; }
  const char* getWavFileName() { return m_wavFile; }
  const char* createElekonFileName();
  enSdRes createTimeFileName(enRecFmt recFmt);
  static enRecFmt getFileFmt();

private:
  static enSdRes createRecordingDir(char* out, size_t outBufSize);
  void saveStartTime();

  bool m_isOpen;
  int m_startY;
  int m_startM;
  int m_startD;
  int m_currY;
  int m_currM;
  int m_currD;
  int m_counter;

  int m_fy;
  int m_fMo;
  int m_fDay;
  int m_fh;
  int m_fMin;
  int m_fSec;

  cXmlHelper m_xml;
  char m_prjName[16];
  char m_wavFile[40];
  char m_name[20];
};

#endif // CEXPORTER_H
