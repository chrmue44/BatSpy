#ifndef C_FILE_INFO_H
#define C_FILE_INFO_H

#include <cstdint>
#include "cSdCard.h"

#define TAG_SAMPLE_RATE "Samplerate"

enum enToken {
  OPEN_TAG,
  TEXT,
  CLOSE_TAG,
  END,
  ERR_TOKEN
};

class cFileInfo {

 public:
  int write(const char* fileName, float duration, int32_t sampleRate, const char* date,
            const char* wavFile, float lat, float lon);
  int readParameter(const char* fileName, uint32_t& sampleRate);

 private:
  int writeLine(const char* text);
  int writeTag(const char* tag, const char* text);
  int writeTag(const char* tag, float val, const char* unit);
  int writeTag(const char* tag, float val1, float val2);
  int writeTag(const char* tag, int32_t val, const char* unit);
  enToken getToken();
  int getNextChar();
  void putBack();
 private:
  tFILE m_file;
  char m_name[80];
};

#endif  //#ifndef C_FILE_INFO_H
