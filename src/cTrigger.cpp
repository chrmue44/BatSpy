/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2023 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************/

#include "globals.h"
//#define DEBUG_LEVEL  4
#include "debug.h"
#include "cTrigger.h"


cTrigger::cTrigger(stFftInfo& info, AudioAnalyzePeak& peak) :
m_fftInfo(info),
m_peak(peak)
{
}

void cTrigger::setMinEventLength(float len, uint32_t sampleRate)
{
  m_minEventLen = (int)(len / 1000.0 * sampleRate / 1024.0 + 0.5);
  if(m_minEventLen == 0)
    m_minEventLen = 1;
}

void cTrigger::checkTrigger(size_t parSet)
{
  float fFilter = devPars.trigFiltFreq[parSet].get() * 1000.0;
  checkRecordingTrigger(fFilter, parSet);
 // checkLiveTrigger(fFilter);
}

void cTrigger::releaseLiveTrigger()
{
  if(m_stateLive != enTrigState::ARMED)
  {
    m_liveTrigger = false;
    m_countLive = 0;
    m_stateLive = enTrigState::ARMED;
    devStatus.lastCallF.set(m_minFreq / 1000.0);
    DPRINTF4("last event: %.1f kHz\n", m_minFreq / 1000.0);
    m_minFreq = 999999;
  }
}

void cTrigger::releaseRecTrigger()
{
  if(m_stateRec != enTrigState::ARMED)
  {
    m_recTrigger = false;
    m_countRec = 0;
    m_stateRec = enTrigState::ARMED;
    if (m_peak.available())
      m_peakVal = m_peak.read();
    DPRINTF4("rec trig released: peak %.3f \n", m_peakVal);
  }
}

void cTrigger::checkRecordingTrigger(float fFilter, size_t parSet)
{
  switch (m_stateRec)
  {
    default:
    case enTrigState::ARMED:
      if (m_peak.available())
        m_peakVal = m_peak.read();
      if (((devPars.triggerType[parSet].get() == enTrigType::LEVEL) && (m_peakVal >= m_recThresh)) ||
       (devPars.checkDebugLevel(DBG_PERM_REC)))
      {
        DPRINTF4("recording triggered, peak %.3f, thresh %.3f\n", m_peakVal, m_recThresh);
        m_recTrigger = true;
        m_stateRec = enTrigState::TRIGGERED;
      }
      else if (devPars.triggerType[parSet].get() == enTrigType::FREQUENCY)
      {
        if ((m_fftInfo.lastMaxAmpl > FREQ_THRESHOLD) && (m_fftInfo.lastMaxFreq >= fFilter))
        {
          m_countRec = 0;
          m_stateRec = enTrigState::PRE_TRIG;
          DPRINTF4("pre trigger f: %f\n", m_fftInfo.lastMaxFreq);
        }
      }
      else if (devPars.triggerType[parSet].get() == enTrigType::FREQ_LEVEL)
      {
        if ((m_fftInfo.lastMaxAmpl > FREQ_THRESHOLD) &&
            (m_fftInfo.lastMaxFreq >= fFilter) && (m_peakVal >= m_recThresh))
        {
          m_countRec = 0;
          m_stateRec = enTrigState::PRE_TRIG;
          DPRINTF4("pre trigger f: %f, level %.3f\n", m_fftInfo.lastMaxFreq, m_peakVal);
        }
      }
      break;

    case enTrigState::PRE_TRIG:
      if((m_fftInfo.lastMaxAmpl > FREQ_THRESHOLD) && (m_fftInfo.lastMaxFreq >= fFilter))
      {
        m_countRec++;
        if(m_countRec >= m_minEventLen)
        {
          m_trigInfo = m_fftInfo;
          DPRINTF4("recording triggered, count %i\n", m_countRec);
          m_recTrigger = true;
          m_stateRec = enTrigState::TRIGGERED;
        }
      }
      else
        m_stateRec = enTrigState::ARMED;
      break;

    case enTrigState::TRIGGERED:
      break;
  }
}

void cTrigger::initTrigLog(const char* dir)
{
  trigLog.close();
  trigLog.setDirectory(dir);
  trigLog.log("fileName, countRec, peakVal, threshold, maxAmpl, maxFreq,avg, avgPeak, bandwidth");
}

void cTrigger::logTrigInfo(const char* fileName)
{
   trigLog.logf(",%s,  %i, %.2f, %.2f, %.1f, %.1f, %.1f, %.1f\n", 
                fileName, m_countRec, m_peakVal, m_recThresh, m_trigInfo.lastMaxAmpl,
                m_trigInfo.lastMaxFreq, m_trigInfo.lastAvg, m_trigInfo.lastAvgPeak, m_trigInfo.bw);
}

void cTrigger::checkLiveTrigger(float fFilter)
{
  switch (m_stateLive)
  {
    default:
    case enTrigState::ARMED:
      if((m_fftInfo.lastMaxAmpl > FREQ_THRESHOLD) && (m_fftInfo.lastMaxFreq >= fFilter))
      {
        m_countLive = 0;
        m_stateLive = enTrigState::PRE_TRIG;
      }
      break;

    case enTrigState::PRE_TRIG:
      if((m_fftInfo.lastMaxAmpl > FREQ_THRESHOLD) && (m_fftInfo.lastMaxFreq >= fFilter))
      {
        m_countLive++;
        if(m_countLive > 0)
        {
          DPRINT4("live display triggered\n");
          m_liveTrigger = true;
          m_stateLive = enTrigState::TRIGGERED;
        }
      }
      else
        m_stateLive = enTrigState::ARMED;
      if((m_fftInfo.lastMaxAmpl > FREQ_THRESHOLD) && (m_fftInfo.lastMaxFreq < m_minFreq))
        m_minFreq = m_fftInfo.lastMaxFreq;
      break;

    case enTrigState::TRIGGERED:
      break;
  }
}