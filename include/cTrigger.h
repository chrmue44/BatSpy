/*************************************************************
 * BatSpy: Teensy 4.1 based recording device for bat sounds  *
 * ***********************************************************
 * Copyright (C) 2023 Christian Mueller                      *
 *                    chrmue44(AT)gmail{DOT}com              *
 * License: GNU GPLv3.0                                      *
 * ***********************************************************
 * A trigger machine to detect relevant audio signals
 * ***********************************************************/
#ifndef CTRIGGER_H
#define CTRIGGER_H
/// status of trigger machine
enum enTrigState
{
  ARMED = 0,
  PRE_TRIG = 1,
  TRIGGERED = 2,
};

#define FREQ_THRESHOLD    150     ///< threshold value for intensity to detect a frequency

// some variables to handles FFT operation
struct stFftInfo
{
  uint32_t lastFftTime;           // last time when fft was calculated
  int liveCnt = 0;                // counter to extend scrolling of live display when signal stopped 
  uint16_t lastMaxAmpl;           // max amplitude of last fft
  float lastMaxFreq;              // max frequency of last fft
};

class cTrigger
{
 private:
  enTrigState m_stateRec = enTrigState::ARMED;  ///< status of recording trigger machine 
  enTrigState m_stateLive = enTrigState::ARMED; ///< status of live trigger machine 
  int m_countRec = 0;                           ///< counter to delay trigger
  int m_countLive = 0;                          ///< counter to delay trigger
  bool m_recTrigger = false;                    ///< trigger signal for recording
  bool m_liveTrigger = false;                   ///< trigger signal for live display
  stFftInfo& m_fftInfo;
  AudioAnalyzePeak& m_peak;                     ///< peak detector
  float m_peakVal;
  float m_recThresh = 0.1;                      ///< recording thresh hold (0.0 ... 1.0)
  float m_minFreq = 15000;                      ///< minimal frequency found during trigger
  int m_minEventLen = 1;                        ///< minimum event length to trigger

 public:
  cTrigger(stFftInfo& info, AudioAnalyzePeak& peak);
  void checkTrigger();
  void releaseLiveTrigger();
  void releaseRecTrigger();
  //AudioAnalyzePeak& getPeakDetector() { return m_peak; } 
  bool getRecTrigger() { return m_recTrigger; }
  bool getLiveTrigger() { return m_liveTrigger; }
  float lastPeakVal() { return m_peakVal;}
  void setThreshold(float level) { m_recThresh = level; }
  void setMinEventLength(float len, uint32_t sampleRate);

 private:
  void checkLiveTrigger(float fFilter);
  void checkRecordingTrigger(float fFilter);

 protected:
  cTrigger();
};

#endif //#ifndef CTRIGGER_H
