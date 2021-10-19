/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * The cFileInfo class provides functionality to create XML
 * info files for recordings. The XML file contains
 * additional information that is not included in the audio
 * recording
 * ***********************************************************/

#ifndef C_FILE_INFO_H
#define C_FILE_INFO_H

#include <cstdint>
#include "cSdCard.h"

#define TAG_SAMPLE_RATE  "Samplerate"
#define TAG_PEAK_VAL     "PeakValue"
#define TAG_DURATION     "Duration"
#define TAG_DATE_TIME    "DateTime"
#define TAG_FILE_NAME    "FileName"
#define TAG_POSITION     "Position"
#define TAG_TRIG_LEVEL   "Level"
#define TAG_TRIG_FILT    "Filter"
#define TAG_TRIG_FREQ    "Frequency"
#define TAG_GAIN         "Gain"
#define TAG_INP_FILTER   "InputFilter"

enum enToken
{
  OPEN_TAG,
  TEXT,
  CLOSE_TAG,
  END,
  ERR_TOKEN
};

class cFileInfo
{

 public:
  int write(const char* fileName, float duration, const char* date,
            const char* wavFile, float lat, float lon, float peakVal);
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
