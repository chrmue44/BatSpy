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

cTrigger::cTrigger(stFftInfo& info) :
m_fftInfo(info),
m_peak()
{

}

void cTrigger::checkTrigger()
{
  float fFilter = devPars.filtFreq.get() * 1000.0;
  checkRecordingTrigger(fFilter);
  checkLiveTrigger(fFilter);
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
  }      
}

void cTrigger::checkRecordingTrigger(float fFilter)
{
  switch (m_stateRec)
  {
    default:
    case enTrigState::ARMED:
      if (m_peak.available())
        m_peakVal = m_peak.read();
      if ((devPars.triggerType.get() == enTrigType::LEVEL) &&
        (m_peakVal >= m_recThresh))
      {
        DPRINT4("recording triggered\n");
        m_recTrigger = true;
        m_stateRec = enTrigState::TRIGGERED;
      }
      else if (devPars.triggerType.get() == enTrigType::FREQUENCY)
      {
        if ((m_fftInfo.lastMaxAmpl > FREQ_THRESHOLD) && (m_fftInfo.lastMaxFreq >= fFilter))
        {
          m_countRec = 0;
          m_stateRec = enTrigState::PRE_TRIG;
          DPRINTF4("pre trigger f: %f\n", m_fftInfo.lastMaxFreq);
        }
      }
      break;

    case enTrigState::PRE_TRIG:
      if((m_fftInfo.lastMaxAmpl > FREQ_THRESHOLD) && (m_fftInfo.lastMaxFreq >= fFilter))
      {
        m_countRec++;
        if(m_countRec > 0)
        {
          DPRINT4("recording triggered\n");
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