/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * Access to the real time clock
 * ***********************************************************/
#ifndef CPRJELEKON_H
#define CPRJELEKON_H

//namespace fs = std::experimental::filesystem;
#include "my_vector.h"
#include "cxmlhelper.h"
#include "types.h"

struct stRecord
{
  char wavFile[40];
  char name[40];
};

class cProject
{
public:
  cProject();
  void createPrjFile(const char* pNotes);
  void closePrjFile();
  void addFile();
  void writeInfoFile(float peakVal, size_t sampleCnt);
  void reset();
  
  bool getIsOpen() { return m_isOpen; }
  size_t getRecCount() { return m_recCount;}
  const char* getWavFileName() { return m_wavFile; }
  const char* createElekonFileName();

private:
  void openExistingPrjFile(const char* fName, int startY, int startM, int startD);
  void initializePrjFile(const char* fName, const char* pNotes, int startY, int startM, int startD);
  void saveStartTime();

  bool m_isOpen;

  int m_fy;
  int m_fMo;
  int m_fDay;
  int m_fh;
  int m_fMin;
  int m_fSec;
  size_t m_recCount = 0;

  cXmlHelper m_xml;
  char m_prjName[16];
  char m_wavFile[128];
  char m_name[30];
};

#endif // CEXPORTER_H
