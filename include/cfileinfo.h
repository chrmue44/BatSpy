#ifndef C_FILE_INFO_H
#define C_FILE_INFO_H

#include <cstdint>
#include "cSdCard.h"

class cFileInfo {

 public:
  int write(const char* fileName, float duration, int32_t sampleRate, const char* date, const char* wavFile);

 private:
  int writeLine(const char* text);
  int writeTag(const char* tag, const char* text);
  int writeTag(const char* tag, float val, const char* unit);
  int writeTag(const char* tag, int32_t val, const char* unit);
 
 private:
  tFILE m_file;
};

#endif  //#ifndef C_FILE_INFO_H
