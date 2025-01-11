/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2021 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "cfileinfo.h"
#include <cSdCard.h>
#include "cutils.h"
#include "globals.h"

#include <cstring>

int MEMF cFileInfo::write(const char* fileName, float duration, const char* date,
                     const char* wavFile, float lat, float lon, float peakVal, float temp, float humid, size_t parSet)
{
  int32_t sampleRate = cAudio::getSampleRateHz((enSampleRate)devPars.sampleRate[parSet].get());
  enSdRes ret = cSdCard::inst().openFile(fileName, m_file, enMode::WRITE);
  if(ret == 0)
  {
    do
    {
      ret = writeLine("<BatRecord>");
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_FILE_NAME, wavFile);
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_DATE_TIME, date);
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_SN, serialNumber);
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_FIRMWARE, devStatus.version.get());
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_SAMPLE_RATE, sampleRate, "Hz");
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_GAIN, devPars.preAmpGain[parSet].getActText());
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_INP_FILTER, "None");
        if(ret != enSdRes::OK) break;
      ret = writeLine("<Trigger>");
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_TRIG_TYPE, devPars.triggerType[parSet].getActText());
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_TRIG_LEVEL, devPars.recThreshhold[parSet].get(), "dB");
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_TRIG_EV_LEN, devPars.minEventLen[parSet].get(), "ms");
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_TRIG_FILT, devPars.trigFiltType[parSet].getActText());
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_TRIG_FREQ, devPars.trigFiltFreq[parSet].get(), "kHz");
        if(ret != enSdRes::OK) break;
      ret = writeLine("</Trigger>");
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_DURATION, duration, "Sec");
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_PEAK_VAL, peakVal, "%");
        if(ret != enSdRes::OK) break;
      ret = writeLine("<GPS>");
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_POSITION, lat, lon);
        if(ret != enSdRes::OK) break;
      ret = writeLine("</GPS>");
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_TEMPERAtURE, devStatus.temperature.get(), "°C");
        if(ret != enSdRes::OK) break;
      ret = writeTag(TAG_HUMIDITY, devStatus.humidity.get(), "%");
        if(ret != enSdRes::OK) break;
      ret = writeLine("</BatRecord>");
    } while(false);    
    ret = cSdCard::inst().closeFile(m_file);
  }
  return ret;
}

enSdRes MEMF cFileInfo::writeLine(const char* text)
{
  enSdRes retVal = enSdRes::OK;
  size_t bytesWritten;
  size_t len = strlen(text);
  retVal = cSdCard::inst().writeFile(m_file, text, bytesWritten, len);
  if(retVal == 0)
    retVal = cSdCard::inst().writeFile(m_file, "\n", bytesWritten, 1);
  return retVal;
}


enSdRes MEMF cFileInfo::writeTag(const char* tag, const char* text)
{
  char buf[128];
  snprintf(buf,sizeof(buf),"<%s>%s</%s>", tag, text, tag);
  buf[127] = 0;
  return writeLine(buf);
}


enSdRes MEMF cFileInfo::writeTag(const char* tag, float val, const char* unit)
{
  char buf[128];
  snprintf(buf,sizeof(buf),"<%s>%f %s</%s>", tag, val, unit, tag);
  buf[127] = 0;
  return writeLine(buf);
}

enSdRes MEMF cFileInfo::writeTag(const char* tag, float val1, float val2)
{
  char buf[128];
  snprintf(buf,sizeof(buf),"<%s>%f %f</%s>", tag, val1, val2, tag);
  buf[127] = 0;
  return writeLine(buf);
}

enSdRes MEMF cFileInfo::writeTag(const char* tag, int32_t val, const char* unit)
{
  char buf[128];
  snprintf(buf,sizeof(buf),"<%s>%li %s</%s>", tag, val, unit, tag);
  buf[127] = 0;
  return writeLine(buf);
}

int MEMF cFileInfo::getNextChar()
{
  char c;
  size_t bytesRead;
  enSdRes ret = cSdCard::inst().readFile(m_file, &c, bytesRead, 1);
  if((bytesRead == 0) || (ret != enSdRes::OK))
    return -1;
  else
    return c;
}

void MEMF cFileInfo::putBack()
{
  size_t pos = cSdCard::inst().getFilePos(m_file);
  if(pos > 0)
    cSdCard::inst().setFilePos(m_file, pos - 1);
}


enToken MEMF cFileInfo::getToken() 
{
  int ret;
  do {
    ret = getNextChar();
  } while (cUtils::isWhiteSpace(ret));
  size_t idx = 0;
  if (ret < 0)
    return enToken::END;

  if(ret == '<') 
  {
    ret = getNextChar();
    if(ret < 0)
      return  enToken::END;

    // close tag
    if(ret == '/') 
    {
        do 
        {
          ret = getNextChar();
          m_name[idx++] = (char)ret;
          if(idx >= sizeof (m_name))
            return enToken::ERR_TOKEN;
        } while ((ret != '>') || (ret < 0));
        if((ret > 0) && (idx > 0))
          m_name[idx - 1] = 0;

        return enToken::CLOSE_TAG;
    }

    //open tag
    else 
    {
      m_name[idx++] = (char)ret;
      do 
      {
        ret = getNextChar();
        m_name[idx++] = (char)ret;
        if(idx >= sizeof (m_name))
          return enToken::ERR_TOKEN;
      } while ((ret != '>') || (ret < 0));
      if((ret > 0) && (idx > 0))
        m_name[idx - 1] = 0;
      return enToken::OPEN_TAG;
    }
  }

  // text
  else 
  {
      m_name[idx++] = (char)ret;
      do 
      {
        ret = getNextChar();
        m_name[idx++] = (char)ret;
        if((idx >= sizeof (m_name)) || (ret < 0))
          return enToken::ERR_TOKEN;
      } while (ret != '<');
      if(idx > 0)
        m_name[idx - 1] = 0;
      if(ret > 0)
        putBack();
      return enToken::TEXT;
  }
  return enToken::ERR_TOKEN;
}

int MEMF cFileInfo::readParameter(const char* fileName, uint32_t& sampleRate) 
{
  int retVal = 0;
  enSdRes res = cSdCard::inst().openFile(fileName, m_file, enMode::READ);
  if(res == enSdRes::OK) 
  {
    do 
    {
      enToken tok = getToken();
      if (tok == enToken::END)
        break;
      if(tok == enToken::OPEN_TAG) 
      {
        if(strcmp(m_name, TAG_SAMPLE_RATE) == 0) 
        {
          tok = getToken();
          if(tok == enToken::TEXT) 
          {
            char* p = strstr(m_name, "Hz");
            if(p == nullptr) 
            {
              retVal = 3;
              break;
            }
            *p = 0;
            sampleRate = atoi(m_name);
            break;
          }
          else 
          {
            retVal = 2;
            break;
          }
        }
      }
    } while(true);
    cSdCard::inst().closeFile(m_file);
  }
  else
    retVal = 1;
  return retVal;
}
