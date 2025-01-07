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
#define TAG_SN           "SN"
#define TAG_FIRMWARE     "Firmware"
#define TAG_FILE_NAME    "FileName"
#define TAG_POSITION     "Position"
#define TAG_TRIG_TYPE    "TriggerType"
#define TAG_TRIG_EV_LEN  "EventLength"
#define TAG_TRIG_LEVEL   "Level"
#define TAG_TRIG_FILT    "Filter"
#define TAG_TRIG_FREQ    "Frequency"
#define TAG_GAIN         "Gain"
#define TAG_INP_FILTER   "InputFilter"
#define TAG_TEMPERAtURE  "Temparature"
#define TAG_HUMIDITY     "Humidity"

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
            const char* wavFile, float lat, float lon, float peakVal, float temp, float humid, size_t parset);
  int readParameter(const char* fileName, uint32_t& sampleRate);

 private:
  enSdRes writeLine(const char* text);
  enSdRes writeTag(const char* tag, const char* text);
  enSdRes writeTag(const char* tag, float val, const char* unit);
  enSdRes writeTag(const char* tag, float val1, float val2);
  enSdRes writeTag(const char* tag, int32_t val, const char* unit);
  enToken getToken();
  int getNextChar();
  void putBack();
 private:
  tFILE m_file;
  char m_name[80];
};

#endif  //#ifndef C_FILE_INFO_H
